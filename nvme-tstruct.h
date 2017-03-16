#undef TSTRUCT_INC_FILE
#define TSTRUCT_INC_FILE nvme-tstruct

#if !defined(NVME_TSTRUCT) || defined(TSTRUCT_HEADER_MULTI_READ)
#define NVME_TSTRUCT

#include "nvme.h"
#include "tstruct.h"

TSTRUCT(nvme_id_power_state,
	TSTRUCT_entries(
		__field(__le16, max_power)
		r_field(__u8, rsvd2)
		__field(__u8, flags)
		__field(__le32, entry_lat)
		__field(__le32, exit_lat)
		__field(__u8, read_tput)
		__field(__u8, read_lat)
		__field(__u8, write_tput)
		__field(__u8, write_lat)
		__field(__le16, idle_power)
		__field(__u8, idle_scale)
		r_field(__u8, rsvd19)
		__field(__le16, active_power)
		__field(__u8, active_work_scale)
		r_array(__u8, rsvd23, 9)
	)
)
TSTRUCT(nvme_id_ctrl,
	TSTRUCT_entries(
		__field(__le16, vid)
		__field(__le16, ssvid)
		__array(char, sn, 20)
		__array(char, mn, 40)
		__array(char, fr, 8)
		__field(__u8, rab)
		__array(__u8, ieee, 3)
		__field(__u8, cmic)
		__field(__u8, mdts)
		__field(__le16, cntlid)
		__field(__le32, ver)
		__field(__le32, rtd3r)
		__field(__le32, rtd3e)
		__field(__le32, oaes)
		__field(__le32, ctratt)
		r_array(__u8, rsvd100, 156)
		__field(__le16, oacs)
		__field(__u8, acl)
		__field(__u8, aerl)
		__field(__u8, frmw)
		__field(__u8, lpa)
		__field(__u8, elpe)
		__field(__u8, npss)
		__field(__u8, avscc)
		__field(__u8, apsta)
		__field(__le16, wctemp)
		__field(__le16, cctemp)
		__field(__le16, mtfa)
		__field(__le32, hmpre)
		__field(__le32, hmmin)
		__array(__u8, tnvmcap, 16)
		__array(__u8, unvmcap, 16)
		__field(__le32, rpmbs)
		r_array(__u8, rsvd316, 4)
		__field(__le16, kas)
		r_array(__u8, rsvd322, 190)
		__field(__u8, sqes)
		__field(__u8, cqes)
		__field(__le16, maxcmd)
		__field(__le32, nn)
		__field(__le16, oncs)
		__field(__le16, fuses)
		__field(__u8, fna)
		__field(__u8, vwc)
		__field(__le16, awun)
		__field(__le16, awupf)
		__field(__u8, nvscc)
		r_field(__u8, rsvd531)
		__field(__le16, acwu)
		r_array(__u8, rsvd534, 2)
		__field(__le32, sgls)
		r_array(__u8, rsvd540, 228)
		__array(char, subnqn, 256)
		r_array(__u8, rsvd1024, 768)
		__field(__le32, ioccsz)
		__field(__le32, iorcsz)
		__field(__le16, icdoff)
		__field(__u8, ctrattr)
		__field(__u8, msdbd)
		r_array(__u8, rsvd1804, 244)
		__array_struct(nvme_id_power_state, psd, 32, npss + 1)
		v_array(__u8, vs, 1024)
	)
)

TSTRUCT(nvme_lbaf,
	__field(__le16, ms)
	__field(__u8, ds)
	__field(__u8, rp)
)
TSTRUCT(nvme_id_ns,
	TSTRUCT_entries(
		l_field(__le64, nsze)
		l_field(__le64, ncap)
		l_field(__le64, nuse)
		__field(__u8, nsfeat)
		__field(__u8, nlbaf)
		__field(__u8, flbas)
		__field(__u8, mc)
		__field(__u8, dpc)
		__field(__u8, dps)
		__field(__u8, nmic)
		__field(__u8, rescap)
		__field(__u8, fpi)
		r_field(__u8, rsvd33)
		__field(__le16, nawun)
		__field(__le16, nawupf)
		__field(__le16, nacwu)
		__field(__le16, nabsn)
		__field(__le16, nabo)
		__field(__le16, nabspf)
		r_field(__le16, rsvd46)
		__array(__u8, nvmcap, 16)
		r_array(__u8, rsvd64, 40)
		__array(__u8, nguid, 16)
		__array(__u8, eui64, 8)
		__array_struct(nvme_lbaf, lbaf, 16, nlbaf + 1)
		r_array(__u8, rsvd192, 192)
		v_array(__u8, vs, 3712)
	)
)

TSTRUCT(nvme_smart_log,
	TSTRUCT_entries(
		__field(__u8, critical_warning)
		__array(__u8, temperature, 2)
		__field(__u8, avail_spare)
		__field(__u8, spare_thresh)
		__field(__u8, percent_used)
		r_array(__u8, rsvd6, 26)
		__array(__u8, data_units_read, 16)
		__array(__u8, data_units_written, 16)
		__array(__u8, host_reads, 16)
		__array(__u8, host_writes, 16)
		__array(__u8, ctrl_busy_time, 16)
		__array(__u8, power_cycles, 16)
		__array(__u8, power_on_hours, 16)
		__array(__u8, unsafe_shutdowns, 16)
		__array(__u8, media_errors, 16)
		__array(__u8, num_err_log_entries, 16)
		__field(__le32, warning_temp_time)
		__field(__le32, critical_comp_time)
		__array(__le16, temp_sensor, 8)
		r_array(__u8, rsvd216, 296)
	)
)

TSTRUCT(nvme_error_log,
	TSTRUCT_entries(
		l_field(__le64, error_count)
		__field(__le16, sqid)
		__field(__le16, cmdid)
		__field(__le16, status_field)
		__field(__le16, parm_error_location)
		l_field(__le64, lba)
		__field(__le32, nsid)
		__field(__u8, vs)
		r_array(__u8, resv, 35)
	)
)

TSTRUCT(nvme_firmware_log,
	TSTRUCT_entries(
		__field(__u8, afi)
		r_array(__u8, resv, 7)
		__array(__le64, frs, 7)
		r_array(__u8, resv2, 448)
	)
)

TSTRUCT(nvme_ns_list,
	TSTRUCT_entries(
		__array(__le32, ns_id, 1024)
	)
)

TSTRUCT(nvme_lba_range_type,
	TSTRUCT_entries(
		__field(__u8, type)
		__field(__u8, attributes)
		r_array(__u8, rsvd2, 14)
		l_field(__u64, slba)
		l_field(__u64, nlb)
		__array(__u8, guid, 16)
		r_array(__u8, rsvd48, 16)
	)
)

TSTRUCT(nvme_registered_ctrl,
	TSTRUCT_entries(
		__field(__le16, cntlid)
		__field(__u8, rcsts)
		r_array(__u8, rsvd3, 5)
		l_field(__le64, hostid)
		l_field(__le64, rkey)
	)
)

TSTRUCT(nvme_reservation_status,
	TSTRUCT_entries(
		__field(__le32, gen)
		__field(__u8, rtype)
		__array(__u8, regctl, 2)
		r_array(__u8, rsvd5, 2)
		__field(__u8, ptpls)
		r_array(__u8, rsvd10, 13)
		__array_struct(nvme_registered_ctrl, regctl_ds, 256, regctl[0])
	)
)

TSTRUCT(nvme_dsm_range,
	TSTRUCT_entries(
		__field(__le32, cattr)
		__field(__le32, nlb)
		l_field(__le64, slba)
	)
)

TSTRUCT(nvme_host_mem_buffer,
	TSTRUCT_entries(
		__field(__u32, hsize)
		__field(__u32, hmdlal)
		__field(__u32, hmdlau)
		__field(__u32, hmdlec)
		r_array(__u8, rsvd16, 4080)
	)
)

TSTRUCT(nvme_auto_pst,
	TSTRUCT_entries(
		__field(__u32, data)
		__field(__u32, rsvd32)
	)
)

TSTRUCT(nvme_controller_list,
	TSTRUCT_entries(
		__field(__le16, num)
		__array(__le16, identifier, 2047)
	)
)

/* Discovery log page entry */
TSTRUCT(nvmf_disc_rsp_page_entry,
	TSTRUCT_entries(
		__field(__u8, trtype)
		__field(__u8, adrfam)
		__field(__u8, subtype)
		__field(__u8, treq)
		__field(__le16, portid)
		__field(__le16, cntlid)
		__field(__le16, asqsz)
		r_array(__u8, resv8, 22)
		__array(char, trsvcid, NVMF_TRSVCID_SIZE)
		r_array(__u8, resv64, 192)
		__array(char, subnqn, NVMF_NQN_FIELD_LEN)
		__array(char, traddr, NVMF_TRADDR_SIZE)
		__array(char, common, NVMF_TSAS_SIZE)
	)
)

/* Discovery log page header */
TSTRUCT(nvmf_disc_rsp_page_hdr,
	TSTRUCT_entries(
		l_field(__le64, genctr)
		l_field(__le64, numrec)
		__field(__le16, recfmt)
		__array(__u8, resv14, 1006)
		__array_struct(nvmf_disc_rsp_page_entry, entries, 0, numrec)
	)
)

TSTRUCT(nvmf_connect_data,
	TSTRUCT_entries(
		__array(__u8, hostid, 16)
		__field(__le16, cntlid)
		r_array(char, resv4, 238)
		__array(char, subsysnqn, NVMF_NQN_FIELD_LEN)
		__array(char, hostnqn, NVMF_NQN_FIELD_LEN)
		r_array(char, resv5, 256)
	)
)

#endif
#include "define_tstruct.h"
