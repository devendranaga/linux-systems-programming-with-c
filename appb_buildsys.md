# Appendix. B  Build systems

###1. [OpenWRT](https://openwrt.org) build system

* The most popular router based OS platform. Runs linux kernel as the main OS.
* Used by many router vendors in their production systems.
* Very simple root file system and provides easy access to the development and test.
* Some missing components that are needed for the production systems (such as CLI, Clean web-interface, one click setup, customization etc). Each vendor implements their own production features to release in the market.
* Has a very versatile and modular build system. Sometimes it is hard to program the build Makefile, but most of the time a copy and paste of a sample Makefile would do the trick.
* The packaging system allows to configure the final software image on to the embedded device allowing it to selectively add or delete software components in the image.
* Allows generation of a firmware image to many target variants: X86, ARM, MIPS, PPC etc. Supports many hardware platforms: Gateworks, i.MX, Broadcom, Marvell etc.
* Allows creation of an SDK for other developers to use and program the system very simply without downloading everything that OpenWRT offers.
* Contains `.config` the single configuration file that contains all the build system information including the kernel, root file system and any other packages. Can be changed by hand or via the `make menuconfig` command.
* A single `make` command or a `make V=99` command would perform the build towards generating the target firmware. The `V=99` is to verbosely build the firmware image.
* To compile a single package `make V=99 package/my-package/compile` would simply build one package.
* To clean a single package `make V=99 package/my-package/clean` would simply clean the given package.
* To clean only the build, perform `make clean`.
* The `opkg` system is the OpenWRT packaging system. Allows the binary and configs etc.. into one single compressed package.
* Has a very good community to support any issues with the OpenWRT.

####1.1 OpenWRT packages

* The OpenWRT's most powerful feature is the packages. Packages add or remove a functionality to the final image.
* They are useful to build a tiny system to a very large miniature computer system. The packages can selectively be chosen to fit the needs of the software project / product.
* Sample package Makefile for the bridge (taken from [here](https://wiki.openwrt.org/doc/devel/packages)).


```makefile
include $(TOPDIR)/rules.mk

PKG_NAME:=bridge
PKG_VERSION:=1.0.6
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/bridge-utils-$(PKG_VERSION)
PKG_SOURCE:=bridge-utils-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/bridge
PKG_MD5SUM:=9b7dc52656f5cbec846a7ba3299f73bd
PKG_CAT:=zcat

include $(INCLUDE_DIR)/package.mk

define Package/bridge
  SECTION:=base
  CATEGORY:=Network
  TITLE:=Ethernet bridging configuration utility
  #DESCRIPTION:=This variable is obsolete. use the Package/name/description define instead!
  URL:=http://bridge.sourceforge.net/
endef

define Package/bridge/description
 Ethernet bridging configuration utility
 Manage ethernet bridging; a way to connect networks together to
 form a larger network.
endef

define Build/Configure
  $(call Build/Configure/Default,--with-linux-headers=$(LINUX_DIR))
endef

define Package/bridge/install
        $(INSTALL_DIR) $(1)/usr/sbin
        $(INSTALL_BIN) $(PKG_BUILD_DIR)/brctl/brctl $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,bridge))
```

###2. LTIB (Linux Target Image builder)

requirements:

Ltib is called linux target image builder. From the website [here](http://ltib.org/home-intro), the ltib is defined as a project tool that can be used to develop and deploy BSPs for a number of embedded platforms including powerpc, arm and coldfire.

This build system is used in most of the freescale and nxp platforms.

To download the ltib you have to run the following:

`cvs -z3 -d:pserver:anonymous@cvs.savannah.nongnu.org:/sources/ltib co -P ltib`

go inside the ltib directory.

`cd ltib`

and then perform the following command.

`./ltib`

The build may fail due to following reasons.

1. need to have rpm, rpmbuild and bison.
2. you might also need to install the perl modules.


on Ubuntu follow the steps:

1. `sudo apt-get install rpm`
2. `sudo apt-get install libwww-perl`
3. `sudo apt-get install`

on Fedora follow the steps:

1. `dnf install perl-libwww-perl`

and then add the following line to /etc/sudoers.

`root ALL = NOPASSWD: /usr/bin/rpm, /opt/ltib/usr/bin/rpm`
