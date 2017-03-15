#include "esxi-intel-nvme.h"

int nvme_submit_admin_passthru(vmk_MgmtUserHandle handle, NvmeAdminCmd *cmd)
{
   return vmk_MgmtUserCallbackInvoke(mgmtHandle, VMK_MGMT_NO_INSTANCE_ID,
                                  MGMT_CALLBACK_ID, cmd, &cmd->result);
}

int nvme_passthru(vmk_MgmtUserHandle handle, __u8 opcode, __u8 flags, __u16 rsvd,
        __u32 nsid, __u32 cdw2, __u32 cdw3, __u32 cdw10, __u32 cdw11,
        __u32 cdw12, __u32 cdw13, __u32 cdw14, __u32 cdw15,
        __u32 data_len, void *data, __u32 metadata_len,
        void *metadata, __u32 timeout_ms, __u32 *result)
{
   NvmeAdminCmd cmd = {
      .opcode      = opcode,
      .flags      = flags,
      .rsvd1      = rsvd,
      .nsid      = nsid,
      .cdw2      = cdw2,
      .cdw3      = cdw3,
      .metadata   = (__u64)(uintptr_t) metadata,
      .addr      = (__u64)(uintptr_t) data,
      .metadata_len   = metadata_len,
      .data_len   = data_len,
      .cdw10      = cdw10,
      .cdw11      = cdw11,
      .cdw12      = cdw12,
      .cdw13      = cdw13,
      .cdw14      = cdw14,
      .cdw15      = cdw15,
      .timeout_ms   = timeout_ms,
      .result      = 0,
   };
   int err;

   err = nvme_submit_admin_passthru(handle, &cmd);
   if (!err && result)
      *result = cmd.result;
   return err;
}

int nvme_identify(vmk_MgmtUserHandle handle, __u32 nsid, __u32 cdw10, void *data)
{
   NvmeAdminCmd cmd = {
      .opcode    = nvme_admin_identify,
      .nsid      = nsid,
      .addr      = (__u64)(uintptr_t) data,
      .data_len  = 0x1000,
      .cdw10     = cdw10,
   };

   return nvme_submit_admin_passthru(handle, &cmd);
}

int nvme_identify_ctrl(vmk_MgmtUserHandle handle, void *data)
{
   return nvme_identify(handle, 0, 1, data);
}

int nvme_identify_ns(vmk_MgmtUserHandle handle, __u32 nsid, bool present, void *data)
{
   int cns = present ? NVME_ID_CNS_NS_PRESENT : NVME_ID_CNS_NS;

   return nvme_identify(handle, nsid, cns, data);
}

int nvme_identify_ns_list(vmk_MgmtUserHandle handle, __u32 nsid, bool all, void *data)
{
   int cns = all ? NVME_ID_CNS_NS_ACTIVE_LIST : NVME_ID_CNS_NS_PRESENT_LIST;

   return nvme_identify(handle, nsid, cns, data);
}

int nvme_identify_ctrl_list(vmk_MgmtUserHandle handle, __u32 nsid, __u16 cntid, void *data)
{
   int cns = nsid ? NVME_ID_CNS_CTRL_NS_LIST : NVME_ID_CNS_CTRL_LIST;

   return nvme_identify(handle, nsid, (cntid << 16) | cns, data);
}

int nvme_get_log(vmk_MgmtUserHandle handle, __u32 nsid, __u8 log_id, __u32 data_len, void *data)
{
   NvmeAdminCmd cmd = {
      .opcode     = nvme_admin_get_log_page,
      .nsid      = nsid,
      .addr      = (__u64)(uintptr_t) data,
      .data_len  = data_len,
   };
   __u32 numd = (data_len >> 2) - 1;
   __u16 numdu = numd >> 16, numdl = numd & 0xffff;

   cmd.cdw10 = log_id | (numdl << 16);
   cmd.cdw11 = numdu;

   return nvme_submit_admin_passthru(handle, &cmd);
}

int nvme_fw_log(vmk_MgmtUserHandle handle, struct nvme_firmware_log_page *fw_log)
{
   return nvme_get_log(handle, 0xffffffff, NVME_LOG_FW_SLOT, sizeof(*fw_log), fw_log);
}

int nvme_error_log(vmk_MgmtUserHandle handle, __u32 nsid, int entries,
         struct nvme_error_log_page *err_log)
{
   return nvme_get_log(handle, nsid, NVME_LOG_ERROR, entries * sizeof(*err_log), err_log);
}

int nvme_smart_log(vmk_MgmtUserHandle handle, __u32 nsid, struct nvme_smart_log *smart_log)
{
   return nvme_get_log(handle, nsid, NVME_LOG_SMART, sizeof(*smart_log), smart_log);
}

int nvme_intel_smart_log(vmk_MgmtUserHandle handle, __u32 nsid,
          struct nvme_additional_smart_log *intel_smart_log)
{
   return nvme_get_log(handle, nsid, 0xca, sizeof(*intel_smart_log),
         intel_smart_log);
}

int nvme_discovery_log(vmk_MgmtUserHandle handle, struct nvmf_disc_rsp_page_hdr *log, __u32 size)
{
   return nvme_get_log(handle, 0, NVME_LOG_DISC, size, log);
}

int nvme_feature(vmk_MgmtUserHandle handle, __u8 opcode, __u32 nsid, __u32 cdw10, __u32 cdw11,
       __u32 data_len, void *data, __u32 *result)
{
   NvmeAdminCmd cmd = {
      .opcode    = opcode,
      .nsid      = nsid,
      .cdw10     = cdw10,
      .cdw11     = cdw11,
      .addr      = (__u64)(uintptr_t) data,
      .data_len  = data_len,
   };
   int err;

   err = nvme_submit_admin_passthru(handle, &cmd);
   if (!err && result)
      *result = cmd.result;
   return err;
}

int nvme_set_feature(vmk_MgmtUserHandle handle, __u32 nsid, __u8 fid, __u32 value, bool save,
           __u32 data_len, void *data, __u32 *result)
{
   __u32 cdw10 = fid | (save ? 1 << 31 : 0);

   return nvme_feature(handle, nvme_admin_set_features, nsid, cdw10, value,
             data_len, data, result);
}

int nvme_get_feature(vmk_MgmtUserHandle handle, __u32 nsid, __u8 fid, __u8 sel, __u32 cdw11,
           __u32 data_len, void *data, __u32 *result)
{
   __u32 cdw10 = fid | sel << 8;

   return nvme_feature(handle, nvme_admin_get_features, nsid, cdw10, cdw11,
             data_len, data, result);
}

int nvme_format(vmk_MgmtUserHandle handle, __u32 nsid, __u8 lbaf, __u8 ses, __u8 pi,
      __u8 pil, __u8 ms, __u32 timeout)
{
   __u32 cdw10 = lbaf | ms << 4 | pi << 5 | pil << 8 | ses << 9;
   NvmeAdminCmd cmd = {
      .opcode     = nvme_admin_format_nvm,
      .nsid       = nsid,
      .cdw10      = cdw10,
      .timeout_ms = timeout,
   };

   return nvme_submit_admin_passthru(handle, &cmd);
}

int nvme_ns_create(vmk_MgmtUserHandle handle, __u64 nsze, __u64 ncap, __u8 flbas,
         __u8 dps, __u8 nmic, __u32 *result)
{
   struct nvme_id_ns ns = {
      .nsze  = (nsze),
      .ncap  = (ncap),
      .flbas = flbas,
      .dps   = dps,
      .nmic  = nmic,
   };
   NvmeAdminCmd cmd = {
      .opcode   = nvme_admin_ns_mgmt,
      .addr     = (__u64)(uintptr_t) ((void *)&ns),
      .cdw10    = 0,
      .data_len = 0x1000,
   };
   int err;

   err = nvme_submit_admin_passthru(handle, &cmd);
   if (!err && result)
      *result = cmd.result;
   return err;
}

int nvme_ns_delete(vmk_MgmtUserHandle handle, __u32 nsid)
{
   NvmeAdminCmd cmd = {
      .opcode = nvme_admin_ns_mgmt,
      .nsid   = nsid,
      .cdw10  = 1,
   };

   return nvme_submit_admin_passthru(handle, &cmd);
}

int nvme_ns_attachment(vmk_MgmtUserHandle handle, __u32 nsid, __u16 num_ctrls, __u16 *ctrlist,
             bool attach)
{
   int i;
   __u8 buf[0x1000];
   struct nvme_controller_list *cntlist =
               (struct nvme_controller_list *)buf;
   NvmeAdminCmd cmd = {
      .opcode   = nvme_admin_ns_attach,
      .nsid     = nsid,
      .addr     = (__u64)(uintptr_t) cntlist,
      .cdw10    = attach ? 0 : 1,
      .data_len = 0x1000,
   };

   memset(buf, 0, sizeof(buf));
   cntlist->num = (num_ctrls);
   for (i = 0; i < num_ctrls; i++)
      cntlist->identifier[i] = (ctrlist[i]);

   return nvme_submit_admin_passthru(handle, &cmd);
}

int nvme_ns_attach_ctrls(vmk_MgmtUserHandle handle, __u32 nsid, __u16 num_ctrls, __u16 *ctrlist)
{
   return nvme_ns_attachment(handle, nsid, num_ctrls, ctrlist, true);
}

int nvme_ns_detach_ctrls(vmk_MgmtUserHandle handle, __u32 nsid, __u16 num_ctrls, __u16 *ctrlist)
{
   return nvme_ns_attachment(handle, nsid, num_ctrls, ctrlist, false);
}

int nvme_fw_download(vmk_MgmtUserHandle handle, __u32 offset, __u32 data_len, void *data)
{
   NvmeAdminCmd cmd = {
      .opcode   = nvme_admin_download_fw,
      .addr     = (__u64)(uintptr_t) data,
      .data_len = data_len,
      .cdw10    = (data_len >> 2) - 1,
      .cdw11    = offset >> 2,
   };

   return nvme_submit_admin_passthru(handle, &cmd);
}

int nvme_fw_activate(vmk_MgmtUserHandle handle, __u8 slot, __u8 action)
{
   NvmeAdminCmd cmd = {
      .opcode = nvme_admin_activate_fw,
      .cdw10  = (action << 3) | slot,
   };

   return nvme_submit_admin_passthru(handle, &cmd);
}

int nvme_sec_send(vmk_MgmtUserHandle handle, __u32 nsid, __u8 nssf, __u16 spsp,
        __u8 secp, __u32 tl, __u32 data_len, void *data, __u32 *result)
{
   NvmeAdminCmd cmd = {
      .opcode   = nvme_admin_security_send,
      .addr     = (__u64)(uintptr_t) data,
      .data_len = data_len,
      .nsid     = nsid,
      .cdw10    = secp << 24 | spsp << 8 | nssf,
      .cdw11    = tl,
   };
   int err;

   err = nvme_submit_admin_passthru(handle, &cmd);
   if (!err && result)
      *result = cmd.result;
   return err;
}

int nvme_sec_recv(vmk_MgmtUserHandle handle, __u32 nsid, __u8 nssf, __u16 spsp,
        __u8 secp, __u32 al, __u32 data_len, void *data, __u32 *result)
{
   NvmeAdminCmd cmd = {
      .opcode   = nvme_admin_security_recv,
      .nsid     = nsid,
      .cdw10    = secp << 24 | spsp << 8 | nssf,
      .cdw11    = al,
      .addr     = (__u64)(uintptr_t) data,
      .data_len = data_len,
   };
   int err;

   err = nvme_submit_admin_passthru(handle, &cmd);
   if (!err && result)
      *result = cmd.result;
   return err;
}
