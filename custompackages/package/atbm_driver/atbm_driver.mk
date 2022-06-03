ATBM_DRIVER_DEPENDENCIES =
ATBM_DRIVER_CONF_OPTS =
ATBM_DRIVER_SITE_METHOD = git
ATBM_DRIVER_SITE = https://github.com/bakueikozo/altobeam_wifi
ATBM_DRIVER_VERSION =  985a7b1d75b06afc05b2a31b252f23b07203865d

define ATBM_DRIVER_BUILD_CMDS
	printenv
        $(MAKE) -C $(@D) CROSS_COMPILE=$(TARGET_CROSS)
endef


define ATBM_DRIVER_INSTALL_TARGET_CMDS
        $(INSTALL) -D -m 0755 $(@D)/driver_install/*.* $(TARGET_DIR)/
endef

$(eval $(generic-package))
