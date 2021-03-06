# This is the root Kconfig
KCONFIG ?= $(RIOTBASE)/Kconfig

# Include tools targets
include $(RIOTMAKE)/tools/kconfiglib.inc.mk

# Include fixdep tool
include $(RIOTMAKE)/tools/fixdep.inc.mk

# Generated dir will contain Kconfig generated configurations
GENERATED_DIR = $(BINDIR)/generated

# The sync dir will contain a tree of header files that represent Kconfig symbols
export KCONFIG_SYNC_DIR = $(GENERATED_DIR)/deps

# This file will contain all generated configuration from kconfig
export KCONFIG_GENERATED_AUTOCONF_HEADER_C = $(GENERATED_DIR)/autoconf.h

# Header for the generated header file
define KCONFIG_AUTOHEADER_HEADER
$(if $(filter MINGW% CYGWIN% MSYS%,$(OS)),/)/* RIOT Configuration File */

endef
export KCONFIG_AUTOHEADER_HEADER

# This file will contain the calculated dependencies formated in Kconfig
export KCONFIG_GENERATED_DEPENDENCIES = $(GENERATED_DIR)/Kconfig.dep

# This file will contain application default configurations
KCONFIG_APP_CONFIG = $(APPDIR)/app.config

# Default and user overwritten configurations
KCONFIG_USER_CONFIG = $(APPDIR)/user.config

# This is the output of the generated configuration. It always mirrors the
# content of KCONFIG_GENERATED_AUTOCONF_HEADER_C, and it is used to load
# configuration symbols to the build system.
KCONFIG_OUT_CONFIG = $(GENERATED_DIR)/out.config

# This file is generated by the GENCONFIG tool. It is similar to the .d
# files generated by GCC, and the idea is the same. We want to re-trigger the
# generation of KCONFIG_OUT_CONFIG and KCONFIG_GENERATED_AUTOCONF_HEADER_C
# whenever a change occurs on one of the previously used Kconfig files.
KCONFIG_OUT_DEP = $(KCONFIG_OUT_CONFIG).d

# Include configuration symbols if available. This allows to check for Kconfig
# symbols in makefiles. Make tries to 'remake' all included files (see
# https://www.gnu.org/software/make/manual/html_node/Remaking-Makefiles.html).
-include $(KCONFIG_OUT_CONFIG)

# Add configurations to merge, in ascendent priority (i.e. a file overrides the
# previous ones).
ifeq (1, $(TEST_KCONFIG))
  # KCONFIG_ADD_CONFIG holds a list of .config files that are merged for the
  # initial configuration. This allows to split configurations in common files
  # and share them among boards or cpus.
  MERGE_SOURCES += $(KCONFIG_ADD_CONFIG)
endif

MERGE_SOURCES += $(wildcard $(KCONFIG_APP_CONFIG))
MERGE_SOURCES += $(wildcard $(KCONFIG_USER_CONFIG))

# Create directory to place generated files
$(GENERATED_DIR): $(if $(MAKE_RESTARTS),,$(CLEAN))
	$(Q)mkdir -p $@

# During migration this checks if Kconfig should run. It will run if any of
# the following is true:
# - A file with '.config' extension is present in the application directory
# - A 'Kconfig' file is present in the application directory
# - A previous configuration file is present (e.g. from a previous call to
#   menuconfig)
# - menuconfig is being called
#
# NOTE: This assumes that Kconfig will not generate any default configurations
# just from the Kconfig files outside the application folder (i.e. module
# configuration via Kconfig is disabled by default). Should this change, the
# check would not longer be valid, and Kconfig would have to run on every
# build.
SHOULD_RUN_KCONFIG ?= $(or $(wildcard $(APPDIR)/*.config), \
                           $(wildcard $(APPDIR)/Kconfig), \
                           $(if $(CLEAN),,$(wildcard $(KCONFIG_OUT_CONFIG))), \
                           $(filter menuconfig, $(MAKECMDGOALS)))

# export variable to make it visible in other Makefiles
export SHOULD_RUN_KCONFIG

ifneq (,$(SHOULD_RUN_KCONFIG))

# Add configuration header to build dependencies
BUILDDEPS += $(KCONFIG_GENERATED_AUTOCONF_HEADER_C) $(FIXDEP)

# Include configuration header when building
CFLAGS += -imacros '$(KCONFIG_GENERATED_AUTOCONF_HEADER_C)'

USEMODULE_W_PREFIX = $(addprefix USEMODULE_,$(USEMODULE))
USEPKG_W_PREFIX = $(addprefix USEPKG_,$(USEPKG))

.PHONY: menuconfig

# Opens the menuconfig interface for configuration of modules using the Kconfig
# system. It will try to update the autoconf.h, which will update if needed
# (i.e. out.config changed).
menuconfig: $(MENUCONFIG) $(KCONFIG_OUT_CONFIG)
	$(Q)KCONFIG_CONFIG=$(KCONFIG_OUT_CONFIG) $(MENUCONFIG) $(KCONFIG)
	$(MAKE) $(KCONFIG_GENERATED_AUTOCONF_HEADER_C)

# Variable used to conditionally depend on KCONFIG_GENERATED_DEPDENDENCIES
# When testing Kconfig module modelling this file is not needed
ifneq (1, $(TEST_KCONFIG))
  GENERATED_DEPENDENCIES_DEP = $(KCONFIG_GENERATED_DEPENDENCIES)
endif

# These rules are not included when only calling `make clean` in
# order to keep the $(BINDIR) directory clean.
ifneq (clean, $(MAKECMDGOALS))

# Build a Kconfig file defining all used modules and packages. This is done by
# defining symbols like 'USEMODULE_<MODULE_NAME>' or USEPKG_<PACKAGE_NAME> which
# default to 'y'. Then, every module and package Kconfig menu will depend on
# that symbol being set to show its options.
# Do nothing when testing Kconfig module dependency modelling.
$(KCONFIG_GENERATED_DEPENDENCIES): FORCE | $(GENERATED_DIR)
	$(Q)printf "%s " $(USEMODULE_W_PREFIX) $(USEPKG_W_PREFIX) \
	  | awk 'BEGIN {RS=" "}{ gsub("-", "_", $$0); \
	      printf "config %s\n\tbool\n\tdefault y\n", toupper($$0)}' \
	  | $(LAZYSPONGE) $(LAZYSPONGE_FLAGS) $@

# Generates a .config file by merging multiple sources specified in
# MERGE_SOURCES. This will also generate KCONFIG_OUT_DEP with the list of used
# Kconfig files.
$(KCONFIG_OUT_CONFIG): $(GENERATED_DEPENDENCIES_DEP) $(GENCONFIG) $(MERGE_SOURCES) | $(GENERATED_DIR)
	$(Q) $(GENCONFIG) \
	  --config-out=$(KCONFIG_OUT_CONFIG) \
	  --file-list $(KCONFIG_OUT_DEP) \
	  --kconfig-filename $(KCONFIG) \
	  --config-sources $(MERGE_SOURCES) && \
	  touch $(KCONFIG_OUT_CONFIG)

endif # eq (clean, $(MAKECMDGOALS))

# Generates the configuration header file which holds the same information
# as KCONFIG_OUT_CONFIG, and is used to inject the configurations during
# compilation.
#
# This will generate the 'dummy' header files needed for incremental builds.
$(KCONFIG_GENERATED_AUTOCONF_HEADER_C): $(KCONFIG_OUT_CONFIG)
	$(Q) $(GENCONFIG) \
	  --header-path $(KCONFIG_GENERATED_AUTOCONF_HEADER_C) \
	  --sync-deps $(KCONFIG_SYNC_DIR) \
	  --kconfig-filename $(KCONFIG) \
	  --config-sources $(KCONFIG_OUT_CONFIG) && \
	  touch $(KCONFIG_GENERATED_AUTOCONF_HEADER_C)

# Try to load the list of Kconfig files used
-include $(KCONFIG_OUT_DEP)

endif
