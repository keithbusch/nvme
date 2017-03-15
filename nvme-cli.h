#undef CMD_INC_FILE
#define CMD_INC_FILE nvme-cli

#if !defined(NVME_BUILTIN) || defined(CMD_HEADER_MULTI_READ)
#define NVME_BUILTIN

#include "cmd.h"

PROGRAM("nvme", "0.1", "short", "long", "usage")

COMMAND_LIST(

COMMAND("list", list, "List all NVMe Namespaces", 
	"List all NVMe Namespaces",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("id-ctrl", id_ctrl, "Identify Controller",
	"NVMe Identify Controller",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	struct nvme_id_ctrl id_ctrl;
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	ret = nvme_identify_ctrl(fd, &id_ctrl);
	if (!ret)
		print_nvme_id_ctrl(&id_ctrl, 0);
	else
		print_nvme_status(command->name, ret);
	)
)
COMMAND("id-ns", id_ns, "Identify Namespace",
	"NVMe Identify Namespace",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	struct nvme_id_ns ns;
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	ret = nvme_identify_ns(fd, 1, false, &ns);
	if (!ret)
		print_nvme_id_ns(&ns, 0);
	else
		print_nvme_status(command->name, ret);
	)
)
COMMAND("list-ns", list_ns, "Identify Namespace List", 
	"NVMe Identify Namespace List",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("list-ctrl", list_ctrl, "Identify Controller List", 
	"NVMe Identify Controller List",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("get-log", get_log, "NVMe Log", 
	"NVMe Log",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("smart-log", smart_log, "Smart Log",
	"NVMe Smart Log",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	struct nvme_smart_log smart_log;
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	ret = nvme_smart_log(fd, 0xffffffff, &smart_log);
	if (!ret)
		print_nvme_smart_log(&smart_log, 0);
	else
		print_nvme_status(command->name, ret);
	)
)
COMMAND("fw-log", get_fw_log, "FW Log", 
	"NVMe FW Log",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	struct nvme_firmware_log fw_log;
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	ret = nvme_fw_log(fd, &fw_log);
	if (!ret)
		print_nvme_firmware_log(&fw_log, 0);
	else
		print_nvme_status(command->name, ret);
	)
)
COMMAND("error-log", get_error_log, "Error Log", 
	"NVMe Error Log",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	struct nvme_error_log err_log[32];
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	ret = nvme_error_log(fd, 0xffffffff, 32, err_log);
	if (!ret)
		print_nvme_error_log(err_log, 0);
	else
		print_nvme_status(command->name, ret);
	)
)
COMMAND("get-feature", get_feature, "Get Feature", 
	"NVMe Get Feature",
	"Example to be written",
	false,
	OPTIONS(
		OPT_U32(feature_id, 'f', "feature-id",
			"Feature identifier")
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	__u32 result;
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	ret = nvme_get_feature(fd, 0, feature_id, 0, 0, 0, NULL,
							&result);
	if (!ret)
		print_nvme_feature(command->name, feature_id, result);
	else
		print_nvme_status(command->name, ret);
	)
)
COMMAND("set-feature", set_feature, "Set Feature", 
	"NVMe Set Feature",
	"Example to be written",
	false,
	OPTIONS(
		OPT_U8(feature_id, 'f', "feature-id", "Feature identifier")
		OPT_U32(value, 'v', "value", "Feature identifier")
		OPT_FMT()
	),
	ACTION(
	__u32 result;
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	ret = nvme_set_feature(fd, 0, feature_id, value, 0, 0, NULL,
							&result);
	if (!ret)
		print_nvme_feature(command->name, feature_id, value);
	else
		print_nvme_status(command->name, ret);
	)
)
COMMAND("format", format, "Format", 
	"NVMe Format",
	"Example to be written",
	false,
	OPTIONS(
		OPT_NS()
		OPT_U8_LIMIT(lbaf, 'l', "lbaf", "LBA format to apply", 15)
		OPT_U8_LIMIT(ses, 's', "ses", "Secure erase", 7)
		OPT_U8_LIMIT(pi, 'i', "pi", "Protection information type", 7)
		OPT_FLAG(pil, 'p', "pil", "Protection information location")
		OPT_FLAG(ms, 'm', "ms", "Use extended/interleaved meta-data")
		OPT_U32(timeout, 't', "timeout", "Command timeout, in milliseconds")
		OPT_FLAG(reset, 'r', "reset", "Reset controller after format")
	),
	ACTION(
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	ret = nvme_format(fd, namespace_id, lbaf, ses, pi, pil, ms, timeout);
	if (!ret) {
		printf("Success formatting namespace:%x\n", namespace_id);
		if (reset)
			ioctl(fd, BLKRRPART);
		else
			nvme_reset_controller(fd);
	} else
		print_nvme_status(command->name, ret);
	)
)
COMMAND("fw-activate", fw_activate, "Firmware Activate", 
	"NVMe Firmware Activate",
	"Example to be written",
	false,
	OPTIONS(
		OPT_U8_LIMIT(slot, 's', "slot", "Firmware slot to activate", 7)
		OPT_U8_LIMIT(action, 'a', "action", "Firmware action", 3)
		OPT_FLAG(reset, 'r', "reset", "Reset controller after activate")
	),
	ACTION(
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	ret = nvme_fw_activate(fd, slot, action);
	switch (ret) {
	case NVME_SC_FW_NEEDS_CONV_RESET:
	case NVME_SC_FW_NEEDS_SUBSYS_RESET:
	case NVME_SC_FW_NEEDS_RESET:
		print_nvme_status(command->name, ret); /* Fall through */
	case NVME_SC_SUCCESS:
		printf("Success activating firmware\n");
		if (reset)
			nvme_reset_controller(fd);
		break;
	default:
		print_nvme_status(command->name, ret);
		break;
	}
	)
)
COMMAND("fw-download", fw_download, "Firmware Download", 
	"NVMe Firmware Download",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FILE_R(fw, 'f', "fw", "Firmware file")
		OPT_U32_INIT(xfer, 'x', "xfter", "Transfer chunk size", 0x1000)
		OPT_U32(offset, 'o', "offset", "Firmware DWord offset")
	),
	ACTION(
	__u8 buf[xfer >= 0x1000 ? xfer : 0x1000];
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	if (xfer < 0x1000)
		xfer = 0x1000;
	while (true) {
		size_t bytes = fread(buf, 1, xfer, fw);

		if (!bytes)
			break;

		ret = nvme_fw_download(fd, offset, bytes, buf);
		if (ret)
			break;
		offset += bytes;
	}
	if (!ret)
		printf("Success downloading firmware\n");
	else
		print_nvme_status(command->name, ret);
	fclose(fw);
	)
)
COMMAND("create-ns", create_ns, "Creates Namespace", 
	"Creates Namespace",
	"Example to be written",
	false,
	OPTIONS(
		OPT_U64(nsze, 's', "nsze", "Namespace size")
		OPT_U64(ncap, 'c', "ncap", "Namespace capacity")
		OPT_U8(flbas, 'f', "flbas", "Formatted LBA size")
		OPT_U8_LIMIT(dps, 'd', "dps", "Data protection settings", 7)
		OPT_U8(nmic, 'm', "nmic", "Multipath and sharing capabilities")
	),
	ACTION(
	__u32 nsid;
	struct nvme_id_ns ns = {
		.nsze		= cpu_to_le64(nsze),
		.ncap		= cpu_to_le64(ncap),
		.flbas		= flbas,
		.dps		= dps,
		.nmic		= nmic,
	};
	int fd = nvme_get_dev(argc, argv);

	if (fd < 0)
		return fd;
	ret = nvme_ns_create(fd, &ns, &nsid);
	if (!ret)
		printf("%s: success, NSID:%d\n", command->name, nsid);
	else
		print_nvme_status(command->name, ret);
	)
)
COMMAND("delete-ns", delete_ns, "Deletes Namespace", 
	"Deletes Namespace",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("attach-ns", attach_ns, "Attaches Namespace", 
	"Attaches Namespace",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("detach-ns", detach_ns, "Detaches Namespace", 
	"Detaches Namespace",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)

COMMAND("admin-passthru", admin_passthru, "NVMe Admin Commmand", 
	"NVMe Admin Commmand",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("io-passthru", io_passthru, "NVMe IO command", 
	"NVMe IO command",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)

COMMAND("security-send", sec_send, "Security Send", 
	"NVMe Security Send",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("security-recv", sec_recv, "Security Receive", 
	"NVMe Security Receive",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)

COMMAND("compare", compare, "Submit a Compare", 
	"Submit a Compare",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("read", read_cmd, "Read", 
	"NVMe Read",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("write", write_cmd, "Write", 
	"NVMe Submit Write",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)

COMMAND("dsm", dsm, "Data Set Management", 
	"NVMe Data Set Management",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("flush", flush, "Flush", 
	"NVMe Flush",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("write-zeroes", write_zeroes, "Write Zeroes", 
	"NVMe Write Zeroes",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("write-uncor", write_uncor, "Write Uncorrectable", 
	"NVMe Write Uncorrectable",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("resv-acquire", resv_acquire, "Reservation Acquire", 
	"NVMe Reservation Acquire",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("resv-register", resv_register, "Reservation Register", 
	"NVMe Reservation Register",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("resv-release", resv_release, "Reservation Release", 
	"NVMe Reservation Release",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("resv-report", resv_report, "Reservation Report", 
	"NVMe Reservation Report",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)

COMMAND("get-ns-id", get_ns_id, "Namespace ID", 
	"NVMe Namespace ID",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("reset", reset, "Resets the controller", 
	"Resets the controller",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("subsystem-reset", subsystem_reset, "Resets the controller", 
	"Resets the controller",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)

COMMAND("show-regs", show_registers, "Shows the controller registers. Requires admin character device", 
	"Shows the controller registers. Requires admin character device",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)

COMMAND("discover", discover_cmd, "Discover NVMeoF subsystems", 
	"Discover NVMeoF subsystems",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("connect-all", connect_all_cmd, "Discover and Connect to NVMeoF subsystems", 
	"Discover and Connect to NVMeoF subsystems",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("connect", connect_cmd, "Connect to NVMeoF subsystem", 
	"Connect to NVMeoF subsystem",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("disconnect", disconnect_cmd, "Disconnect from NVMeoF subsystem", 
	"Disconnect from NVMeoF subsystem",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)
COMMAND("gen-hostnqn", gen_hostnqn_cmd, "Generate NVMeoF host NQN", 
	"Generate NVMeoF host NQN",
	"Example to be written",
	false,
	OPTIONS(
		OPT_FMT()
		OPT_VERBOSE()
	),
	ACTION(
	)
)

)
#endif

#include "define_cmd.h"
