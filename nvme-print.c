#include <endian.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TSTRUCT_SOURCE
#include "nvme-print.h"

#include "nvme.h"

static char *nvme_status_to_string(uint32_t status)
{
	switch (status & 0x3ff) {
	case NVME_SC_SUCCESS:			return "SUCCESS";
	case NVME_SC_INVALID_OPCODE:		return "INVALID_OPCODE";
	case NVME_SC_INVALID_FIELD:		return "INVALID_FIELD";
	case NVME_SC_CMDID_CONFLICT:		return "CMDID_CONFLICT";
	case NVME_SC_DATA_XFER_ERROR:		return "DATA_XFER_ERROR";
	case NVME_SC_POWER_LOSS:		return "POWER_LOSS";
	case NVME_SC_INTERNAL:			return "INTERNAL";
	case NVME_SC_ABORT_REQ:			return "ABORT_REQ";
	case NVME_SC_ABORT_QUEUE:		return "ABORT_QUEUE";
	case NVME_SC_FUSED_FAIL:		return "FUSED_FAIL";
	case NVME_SC_FUSED_MISSING:		return "FUSED_MISSING";
	case NVME_SC_INVALID_NS:		return "INVALID_NS";
	case NVME_SC_CMD_SEQ_ERROR:		return "CMD_SEQ_ERROR";
	case NVME_SC_LBA_RANGE:			return "LBA_RANGE";
	case NVME_SC_CAP_EXCEEDED:		return "CAP_EXCEEDED";
	case NVME_SC_NS_NOT_READY:		return "NS_NOT_READY";
	case NVME_SC_RESERVATION_CONFLICT:	return "RESERVATION_CONFLICT";
	case NVME_SC_CQ_INVALID:		return "CQ_INVALID";
	case NVME_SC_QID_INVALID:		return "QID_INVALID";
	case NVME_SC_QUEUE_SIZE:		return "QUEUE_SIZE";
	case NVME_SC_ABORT_LIMIT:		return "ABORT_LIMIT";
	case NVME_SC_ABORT_MISSING:		return "ABORT_MISSING";
	case NVME_SC_ASYNC_LIMIT:		return "ASYNC_LIMIT";
	case NVME_SC_FIRMWARE_SLOT:		return "FIRMWARE_SLOT";
	case NVME_SC_FIRMWARE_IMAGE:		return "FIRMWARE_IMAGE";
	case NVME_SC_INVALID_VECTOR:		return "INVALID_VECTOR";
	case NVME_SC_INVALID_LOG_PAGE:		return "INVALID_LOG_PAGE";
	case NVME_SC_INVALID_FORMAT:		return "INVALID_FORMAT";
	case NVME_SC_FW_NEEDS_CONV_RESET:	return "FW_NEEDS_CONVENTIONAL_RESET";
	case NVME_SC_INVALID_QUEUE:		return "INVALID_QUEUE";
	case NVME_SC_FEATURE_NOT_SAVEABLE:	return "FEATURE_NOT_SAVEABLE";
	case NVME_SC_FEATURE_NOT_CHANGEABLE:	return "FEATURE_NOT_CHANGEABLE";
	case NVME_SC_FEATURE_NOT_PER_NS:	return "FEATURE_NOT_PER_NS";
	case NVME_SC_FW_NEEDS_SUBSYS_RESET:	return "FW_NEEDS_SUBSYSTEM_RESET";
	case NVME_SC_FW_NEEDS_RESET:		return "FW_NEEDS_RESET";
	case NVME_SC_FW_NEEDS_MAX_TIME:		return "FW_NEEDS_MAX_TIME_VIOLATION";
	case NVME_SC_FW_ACIVATE_PROHIBITED:	return "FW_ACTIVATION_PROHIBITED";
	case NVME_SC_OVERLAPPING_RANGE:		return "OVERLAPPING_RANGE";
	case NVME_SC_NS_INSUFFICENT_CAP:	return "NS_INSUFFICIENT_CAPACITY";
	case NVME_SC_NS_ID_UNAVAILABLE:		return "NS_ID_UNAVAILABLE";
	case NVME_SC_NS_ALREADY_ATTACHED:	return "NS_ALREADY_ATTACHED";
	case NVME_SC_NS_IS_PRIVATE:		return "NS_IS_PRIVATE";
	case NVME_SC_NS_NOT_ATTACHED:		return "NS_NOT_ATTACHED";
	case NVME_SC_THIN_PROV_NOT_SUPP:	return "THIN_PROVISIONING_NOT_SUPPORTED";
	case NVME_SC_CTRL_LIST_INVALID:		return "CONTROLLER_LIST_INVALID";
	case NVME_SC_BAD_ATTRIBUTES:		return "BAD_ATTRIBUTES";
	case NVME_SC_WRITE_FAULT:		return "WRITE_FAULT";
	case NVME_SC_READ_ERROR:		return "READ_ERROR";
	case NVME_SC_GUARD_CHECK:		return "GUARD_CHECK";
	case NVME_SC_APPTAG_CHECK:		return "APPTAG_CHECK";
	case NVME_SC_REFTAG_CHECK:		return "REFTAG_CHECK";
	case NVME_SC_COMPARE_FAILED:		return "COMPARE_FAILED";
	case NVME_SC_ACCESS_DENIED:		return "ACCESS_DENIED";
	case NVME_SC_UNWRITTEN_BLOCK:		return "UNWRITTEN_BLOCK";
	default:				return "Unknown";
	}
}

void print_nvme_status(const char *cmd, int status)
{
	if (status < 0)
		perror(cmd);
	else
		fprintf(stderr, "%s: NVMe Status:%s(%x)\n", cmd,
			nvme_status_to_string(status), status);
}

static char *nvme_feature_to_string(int feature)
{
	switch (feature) {
	case NVME_FEAT_ARBITRATION:	return "Arbitration";
	case NVME_FEAT_POWER_MGMT:	return "Power Management";
	case NVME_FEAT_LBA_RANGE:	return "LBA Range Type";
	case NVME_FEAT_TEMP_THRESH:	return "Temperature Threshold";
	case NVME_FEAT_ERR_RECOVERY:	return "Error Recovery";
	case NVME_FEAT_VOLATILE_WC:	return "Volatile Write Cache";
	case NVME_FEAT_NUM_QUEUES:	return "Number of Queues";
	case NVME_FEAT_IRQ_COALESCE:	return "Interrupt Coalescing";
	case NVME_FEAT_IRQ_CONFIG: 	return "Interrupt Vector Configuration";
	case NVME_FEAT_WRITE_ATOMIC:	return "Write Atomicity Normal";
	case NVME_FEAT_ASYNC_EVENT:	return "Async Event Configuration";
	case NVME_FEAT_AUTO_PST:	return "Autonomous Power State Transition";
	case NVME_FEAT_HOST_MEM_BUF:	return "Host Memory Buffer";
	case NVME_FEAT_SW_PROGRESS:	return "Software Progress";
	case NVME_FEAT_HOST_ID:		return "Host Identifier";
	case NVME_FEAT_RESV_MASK:	return "Reservation Notification Mask";
	case NVME_FEAT_RESV_PERSIST:	return "Reservation Persistence";
	default:			return "Unknown";
	}
}

void print_nvme_feature(const char *cmd, __u32 feature_id, __u32 result)
{
	printf("%s:%02x (%s) value:%08x\n", cmd, feature_id,
		nvme_feature_to_string(feature_id), result);
}
