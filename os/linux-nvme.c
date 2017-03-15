#include <endian.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <endian.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <inttypes.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <linux/types.h>

#include "nvme-ioctl.h"
#include "linux-nvme.h"

int nvme_subsystem_reset(int fd)
{
	return ioctl(fd, NVME_IOCTL_SUBSYS_RESET);
}

int nvme_reset_controller(int fd)
{
	return ioctl(fd, NVME_IOCTL_RESET);
}

int nvme_get_nsid(int fd)
{
	return ioctl(fd, NVME_IOCTL_ID);
}

static int nvme_submit_passthru(int fd, int ioctl_cmd, struct nvme_passthru_cmd *cmd)
{
	return ioctl(fd, ioctl_cmd, cmd);
}

static int nvme_submit_admin_passthru(int fd, struct nvme_passthru_cmd *cmd)
{
	return ioctl(fd, NVME_IOCTL_ADMIN_CMD, cmd);
}

static int nvme_submit_io_passthru(int fd, struct nvme_passthru_cmd *cmd)
{
	return ioctl(fd, NVME_IOCTL_IO_CMD, cmd);
}

static int nvme_passthru(int fd, int ioctl_cmd, __u8 opcode, __u8 flags,
		__u16 rsvd, __u32 nsid, __u32 cdw2, __u32 cdw3, __u32 cdw10,
		__u32 cdw11, __u32 cdw12, __u32 cdw13, __u32 cdw14, __u32 cdw15,
		__u32 data_len, void *data, __u32 metadata_len, void *metadata,
		__u32 timeout_ms, __u32 *result)
{
	struct nvme_passthru_cmd cmd = {
		.opcode		= opcode,
		.flags		= flags,
		.rsvd1		= rsvd,
		.nsid		= nsid,
		.cdw2		= cdw2,
		.cdw3		= cdw3,
		.metadata	= (__u64)(uintptr_t)metadata,
		.addr		= (__u64)(uintptr_t)data,
		.metadata_len	= metadata_len,
		.data_len	= data_len,
		.cdw10		= cdw10,
		.cdw11		= cdw11,
		.cdw12		= cdw12,
		.cdw13		= cdw13,
		.cdw14		= cdw14,
		.cdw15		= cdw15,
		.timeout_ms	= timeout_ms,
		.result		= 0,
	};
	int err;

	err = nvme_submit_passthru(fd, ioctl_cmd, &cmd);
	if (!err && result)
		*result = cmd.result;
	return err;
}

static int nvme_io(int fd, __u8 opcode, __u64 slba, __u16 nblocks,
		__u16 control, __u32 dsmgmt, __u32 reftag, __u16 apptag,
		__u16 appmask, void *data, void *metadata)
{
	struct nvme_user_io io = {
		.opcode		= opcode,
		.flags		= 0,
		.control	= control,
		.nblocks	= nblocks,
		.rsvd		= 0,
		.metadata	= (__u64)(uintptr_t)metadata,
		.addr		= (__u64)(uintptr_t)data,
		.slba		= slba,
		.dsmgmt		= dsmgmt,
		.reftag		= reftag,
		.appmask	= apptag,
		.apptag		= appmask,
	};

	return ioctl(fd, NVME_IOCTL_SUBMIT_IO, &io);
}

int nvme_read(int fd, __u64 slba, __u16 nblocks, __u16 control, __u32 dsmgmt,
	      __u32 reftag, __u16 apptag, __u16 appmask, void *data,
	      void *metadata)
{
	return nvme_io(fd, nvme_cmd_read, slba, nblocks, control, dsmgmt,
		       reftag, apptag, appmask, data, metadata);
}

int nvme_write(int fd, __u64 slba, __u16 nblocks, __u16 control, __u32 dsmgmt,
	       __u32 reftag, __u16 apptag, __u16 appmask, void *data,
	       void *metadata)
{
	return nvme_io(fd, nvme_cmd_write, slba, nblocks, control, dsmgmt,
		       reftag, apptag, appmask, data, metadata);
}

int nvme_compare(int fd, __u64 slba, __u16 nblocks, __u16 control, __u32 dsmgmt,
		 __u32 reftag, __u16 apptag, __u16 appmask, void *data,
		 void *metadata)
{
	return nvme_io(fd, nvme_cmd_compare, slba, nblocks, control, dsmgmt,
		       reftag, apptag, appmask, data, metadata);
}

int nvme_passthru_io(int fd, __u8 opcode, __u8 flags, __u16 rsvd,
		     __u32 nsid, __u32 cdw2, __u32 cdw3, __u32 cdw10,
		     __u32 cdw11, __u32 cdw12, __u32 cdw13, __u32 cdw14,
		     __u32 cdw15, __u32 data_len, void *data,
		     __u32 metadata_len, void *metadata, __u32 timeout_ms)
{
	return nvme_passthru(fd, NVME_IOCTL_IO_CMD, opcode, flags, rsvd, nsid,
			     cdw2, cdw3, cdw10, cdw11, cdw12, cdw13, cdw14,
			     cdw15, data_len, data, metadata_len, metadata,
			     timeout_ms, NULL);
}

int nvme_write_zeros(int fd, __u32 nsid, __u64 slba, __u16 nlb,
		     __u16 control, __u32 reftag, __u16 apptag, __u16 appmask)
{
	struct nvme_passthru_cmd cmd = {
		.opcode		= nvme_cmd_write_zeroes,
		.nsid		= nsid,
		.cdw10		= slba & 0xffffffff,
		.cdw11		= slba >> 32,
		.cdw12		= nlb | (control << 16),
		.cdw14		= reftag,
		.cdw15		= apptag | (appmask << 16),
	};

	return nvme_submit_io_passthru(fd, &cmd);
}

int nvme_write_uncorrectable(int fd, __u32 nsid, __u64 slba, __u16 nlb)
{
	struct nvme_passthru_cmd cmd = {
		.opcode		= nvme_cmd_write_uncor,
		.nsid		= nsid,
		.cdw10		= slba & 0xffffffff,
		.cdw11		= slba >> 32,
		.cdw12		= nlb,
	};

	return nvme_submit_io_passthru(fd, &cmd);
}

int nvme_flush(int fd, __u32 nsid)
{
	struct nvme_passthru_cmd cmd = {
		.opcode		= nvme_cmd_flush,
		.nsid		= nsid,
	};

	return nvme_submit_io_passthru(fd, &cmd);
}

int nvme_resv_acquire(int fd, __u32 nsid, __u8 rtype, __u8 racqa,
		      bool iekey, __u64 crkey, __u64 nrkey)
{
	__le64 payload[2] = { cpu_to_le64(crkey), cpu_to_le64(nrkey) };
	__u32 cdw10 = racqa | (iekey ? 1 << 3 : 0) | rtype << 8;

	struct nvme_passthru_cmd cmd = {
		.opcode		= nvme_cmd_resv_acquire,
		.nsid		= nsid,
		.cdw10		= cdw10,
		.addr		= (__u64)(uintptr_t)payload,
		.data_len	= sizeof(payload),
	};

	return nvme_submit_io_passthru(fd, &cmd);
}

int nvme_resv_register(int fd, __u32 nsid, __u8 rrega, __u8 cptpl,
		       bool iekey, __u64 crkey, __u64 nrkey)
{
	__le64 payload[2] = { cpu_to_le64(crkey), cpu_to_le64(nrkey) };
	__u32 cdw10 = rrega | (iekey ? 1 << 3 : 0) | cptpl << 30;

	struct nvme_passthru_cmd cmd = {
		.opcode		= nvme_cmd_resv_register,
		.nsid		= nsid,
		.cdw10		= cdw10,
		.addr		= (__u64)(uintptr_t)(payload),
		.data_len	= sizeof(payload),
	};

	return nvme_submit_io_passthru(fd, &cmd);
}

int nvme_resv_release(int fd, __u32 nsid, __u8 rtype, __u8 rrela,
		      bool iekey, __u64 crkey)
{
	__le64 payload[1] = { cpu_to_le64(crkey) };
	__u32 cdw10 = rrela | (iekey ? 1 << 3 : 0) | rtype << 8;

	struct nvme_passthru_cmd cmd = {
		.opcode		= nvme_cmd_resv_release,
		.nsid		= nsid,
		.cdw10		= cdw10,
		.addr		= (__u64)(uintptr_t)(payload),
		.data_len	= sizeof(payload),
	};

	return nvme_submit_io_passthru(fd, &cmd);
}

int nvme_resv_report(int fd, __u32 nsid, __u32 numd, void *data)
{
	struct nvme_passthru_cmd cmd = {
		.opcode		= nvme_cmd_resv_report,
		.nsid		= nsid,
		.cdw10		= numd,
		.addr		= (__u64)(uintptr_t)data,
		.data_len	= numd << 2,
	};

	return nvme_submit_io_passthru(fd, &cmd);
}

int nvme_passthru_admin(int fd, __u8 opcode, __u8 flags, __u16 rsvd, __u32 nsid,
		        __u32 cdw2, __u32 cdw3, __u32 cdw10, __u32 cdw11,
		        __u32 cdw12, __u32 cdw13, __u32 cdw14, __u32 cdw15,
		        __u32 data_len, void *data, __u32 metadata_len, void *metadata,
		        __u32 timeout_ms)
{
	return nvme_passthru(fd, NVME_IOCTL_ADMIN_CMD, opcode, flags, rsvd,
			     nsid, cdw2, cdw3, cdw10, cdw11, cdw12, cdw13,
			     cdw14, cdw15, data_len, data, metadata_len,
			     metadata, timeout_ms, NULL);
}

int nvme_identify(int fd, __u32 nsid, __u32 cdw10, void *data)
{
	struct nvme_admin_cmd cmd = {
		.opcode		= nvme_admin_identify,
		.nsid		= nsid,
		.cdw10		= cdw10,
		.addr		= (__u64)(uintptr_t)data,
		.data_len	= 0x1000,
	};

	return nvme_submit_admin_passthru(fd, &cmd);
}

int nvme_identify_ctrl(int fd, void *data)
{
	return nvme_identify(fd, 0, 1, data);
}

int nvme_identify_ns(int fd, __u32 nsid, bool present, void *data)
{
	int cns = present ? NVME_ID_CNS_NS_PRESENT : NVME_ID_CNS_NS;

	return nvme_identify(fd, nsid, cns, data);
}

int nvme_identify_ns_list(int fd, __u32 nsid, bool all, void *data)
{
	int cns = all ? NVME_ID_CNS_NS_ACTIVE_LIST : NVME_ID_CNS_NS_PRESENT_LIST;

	return nvme_identify(fd, nsid, cns, data);
}

int nvme_identify_ctrl_list(int fd, __u32 nsid, __u16 cntid, void *data)
{
	int cns = nsid ? NVME_ID_CNS_CTRL_NS_LIST : NVME_ID_CNS_CTRL_LIST;

	return nvme_identify(fd, nsid, (cntid << 16) | cns, data);
}

int nvme_get_log(int fd, __u32 nsid, __u8 log_id, __u32 data_len, void *data)
{
	__u32 numd = (data_len >> 2) - 1;
	__u16 numdu = numd >> 16;
	__u16 numdl = numd & 0xffff;

	struct nvme_admin_cmd cmd = {
		.opcode		= nvme_admin_get_log_page,
		.nsid		= nsid,
		.cdw10		= log_id | (numdl << 16),
		.cdw11		= numdu,
		.addr		= (__u64)(uintptr_t)data,
		.data_len	= data_len,
	};

	return nvme_submit_admin_passthru(fd, &cmd);
}

int nvme_fw_log(int fd, void *fw_log)
{
	return nvme_get_log(fd, 0xffffffff, NVME_LOG_FW_SLOT, 512, fw_log);
}

int nvme_error_log(int fd, __u32 nsid, int entries,
		   void *err_log)
{
	return nvme_get_log(fd, nsid, NVME_LOG_ERROR, entries * 64, err_log);
}

int nvme_smart_log(int fd, __u32 nsid, void *smart_log)
{
	return nvme_get_log(fd, nsid, NVME_LOG_SMART, 512, smart_log);
}

int nvme_discovery_log(int fd, void *log, __u32 size)
{
	return nvme_get_log(fd, 0, NVME_LOG_DISC, size, log);
}

static int nvme_feature(int fd, __u8 opcode, __u32 nsid, __u32 cdw10, __u32 cdw11,
		 __u32 data_len, void *data, __u32 *result)
{
	struct nvme_admin_cmd cmd = {
		.opcode		= opcode,
		.nsid		= nsid,
		.cdw10		= cdw10,
		.cdw11		= cdw11,
		.addr		= (__u64)(uintptr_t)data,
		.data_len	= data_len,
	};
	int err;

	err = nvme_submit_admin_passthru(fd, &cmd);
	if (!err && result)
		*result = cmd.result;
	return err;
}

int nvme_set_feature(int fd, __u32 nsid, __u8 fid, __u32 value, bool save,
		     __u32 data_len, void *data, __u32 *result)
{
	__u32 cdw10 = fid | (save ? 1 << 31 : 0);

	return nvme_feature(fd, nvme_admin_set_features, nsid, cdw10, value,
			    data_len, data, result);
}

int nvme_get_feature(int fd, __u32 nsid, __u8 fid, __u8 sel, __u32 cdw11,
		     __u32 data_len, void *data, __u32 *result)
{
	__u32 cdw10 = fid | sel << 8;

	return nvme_feature(fd, nvme_admin_get_features, nsid, cdw10, cdw11,
			    data_len, data, result);
}

int nvme_format(int fd, __u32 nsid, __u8 lbaf, __u8 ses, __u8 pi,
		__u8 pil, __u8 ms, __u32 timeout)
{
	__u32 cdw10 = lbaf | ms << 4 | pi << 5 | pil << 8 | ses << 9;
	struct nvme_admin_cmd cmd = {
		.opcode		= nvme_admin_format_nvm,
		.nsid		= nsid,
		.cdw10		= cdw10,
		.timeout_ms	= timeout,
	};

	return nvme_submit_admin_passthru(fd, &cmd);
}

int nvme_ns_create(int fd, void *ns, __u32 *result)
{
	struct nvme_admin_cmd cmd = {
		.opcode		= nvme_admin_ns_mgmt,
		.addr		= (__u64)(uintptr_t)ns,
		.data_len	= 0x1000,
	};
	int err;

	err = nvme_submit_admin_passthru(fd, &cmd);
	if (!err && result)
		*result = cmd.result;
	return err;
}

int nvme_ns_delete(int fd, __u32 nsid)
{
	struct nvme_admin_cmd cmd = {
		.opcode		= nvme_admin_ns_mgmt,
		.nsid		= nsid,
		.cdw10		= 1,
	};

	return nvme_submit_admin_passthru(fd, &cmd);
}

static int nvme_ns_attachment(int fd, __u32 nsid, void *cntlist, bool attach)
{
	struct nvme_admin_cmd cmd = {
		.opcode		= nvme_admin_ns_attach,
		.nsid		= nsid,
		.cdw10		= attach ? 0 : 1,
		.addr		= (__u64)(uintptr_t)cntlist,
		.data_len	= 0x1000,
	};

	return nvme_submit_admin_passthru(fd, &cmd);
}

int nvme_ns_attach_ctrls(int fd, __u32 nsid, void *ctrlist)
{
	return nvme_ns_attachment(fd, nsid, ctrlist, true);
}

int nvme_ns_detach_ctrls(int fd, __u32 nsid, void *ctrlist)
{
	return nvme_ns_attachment(fd, nsid, ctrlist, false);
}

int nvme_fw_download(int fd, __u32 offset, __u32 data_len, void *data)
{
	struct nvme_admin_cmd cmd = {
		.opcode		= nvme_admin_download_fw,
		.cdw10		= (data_len >> 2) - 1,
		.cdw11		= offset >> 2,
		.addr		= (__u64)(uintptr_t)data,
		.data_len	= data_len,
	};

	return nvme_submit_admin_passthru(fd, &cmd);
}

int nvme_fw_activate(int fd, __u8 slot, __u8 action)
{
	struct nvme_admin_cmd cmd = {
		.opcode		= nvme_admin_activate_fw,
		.cdw10		= (action << 3) | slot,
	};

	return nvme_submit_admin_passthru(fd, &cmd);
}

int nvme_sec_send(int fd, __u32 nsid, __u8 nssf, __u16 spsp, __u8 secp,
		  __u32 tl, __u32 data_len, void *data, __u32 *result)
{
	struct nvme_admin_cmd cmd = {
		.opcode		= nvme_admin_security_send,
		.nsid		= nsid,
		.cdw10		= secp << 24 | spsp << 8 | nssf,
		.cdw11		= tl,
		.addr		= (__u64)(uintptr_t)data,
		.data_len	= data_len,
	};
	int err;

	err = nvme_submit_admin_passthru(fd, &cmd);
	if (!err && result)
		*result = cmd.result;
	return err;
}

int nvme_sec_recv(int fd, __u32 nsid, __u8 nssf, __u16 spsp, __u8 secp,
		  __u32 al, __u32 data_len, void *data, __u32 *result)
{
	struct nvme_admin_cmd cmd = {
		.opcode		= nvme_admin_security_recv,
		.nsid		= nsid,
		.cdw10		= secp << 24 | spsp << 8 | nssf,
		.cdw11		= al,
		.addr		= (__u64)(uintptr_t)data,
		.data_len	= data_len,
	};
	int err;

	err = nvme_submit_admin_passthru(fd, &cmd);
	if (!err && result)
		*result = cmd.result;
	return err;
}
