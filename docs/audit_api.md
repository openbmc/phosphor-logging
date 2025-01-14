# Requirements and Dependencies

- Set CONFIG_AUDIT into the kernel config file: CONFIG_AUDIT=y

- Add "auditd" daemon to the corresponding build into meta*/*.bb file:
  RDEPENDS:${PN}-system += " auditd"
