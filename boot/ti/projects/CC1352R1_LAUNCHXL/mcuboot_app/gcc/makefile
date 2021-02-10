SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR ?= /Applications/ti/simplelink_cc13x2_26x2_sdk_4_40_00_44

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

GIT_ROOT = ../../../../../../
APP_ROOT = ${GIT_ROOT}/boot/ti/source/mcuboot_app
PLATFORM = ${GIT_ROOT}/boot/ti/source/platform/CC1352R1_LAUNCHXL

OBJECTS = build/obj/mcuboot_app.obj build/obj/main_nortos.obj build/obj/trace.obj build/obj/flash_map_backend.obj build/obj/ti_drivers_config.obj build/obj/ti_devices_config.obj build/obj/boot_record.obj build/obj/image_ed25519.obj build/obj/bootutil_misc.obj build/obj/image_rsa.obj build/obj/image_validate.obj build/obj/bootutil_public.obj build/obj/loader.obj build/obj/caps.obj build/obj/swap_misc.obj build/obj/encrypted.obj build/obj/swap_move.obj build/obj/fault_injection_hardening.obj build/obj/fault_injection_hardening_delay_rng_mbedtls.obj build/obj/swap_scratch.obj build/obj/image_ec.obj build/obj/tlv.obj build/obj/image_ec256.obj build/obj/aes_decrypt.obj build/obj/ccm_mode.obj build/obj/ctr_prng.obj build/obj/ecc_dsa.obj build/obj/hmac_prng.obj build/obj/aes_encrypt.obj build/obj/cmac_mode.obj build/obj/ecc.obj build/obj/ecc_platform_specific.obj build/obj/sha256.obj build/obj/cbc_mode.obj build/obj/ctr_mode.obj build/obj/ecc_dh.obj build/obj/hmac.obj build/obj/utils.obj

NAME = mcuboot_app

CFLAGS = -I.. \
    -I. \
    -DDeviceFamily_CC13X2 \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    "-I$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos/posix" \
	"-I${GIT_ROOT}/boot/bootutil/include" \
	"-I${GIT_ROOT}/ext/tinycrypt/lib/include" \
	"-I${APP_ROOT}" \
	"-I${PLATFORM}" \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -std=c99 \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -ffunction-sections \
    -fdata-sections \
    -g \
    -gstrict-dwarf \
    -Wall \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

LFLAGS = -Wl,-T,../gcc/cc13x2_cc26x2_nortos.lds \
    "-Wl,-Map,build/$(NAME).map" \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/source" \
    -eresetISR \
    "-L$(SIMPLELINK_CC13X2_26X2_SDK_INSTALL_DIR)/kernel/nortos" \
    -l:ti/drivers/lib/gcc/m4f/drivers_cc13x2.a \
    -l:lib/gcc/m4f/nortos_cc13x2.a \
    -l:ti/devices/cc13x2_cc26x2/driverlib/bin/gcc/driverlib.lib \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -nostartfiles \
    -static \
    -Wl,--gc-sections \
    "-L$(GCC_ARMCOMPILER)/arm-none-eabi/lib/thumb/v7e-m/hard" \
    -lgcc \
    -lc \
    -lm \
    -lnosys \
    --specs=nano.specs

all: build/$(NAME).out

build/obj/mcuboot_app.obj: ${APP_ROOT}/mcuboot_app.c
	@ mkdir -p build
	@ mkdir -p build/obj
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/main_nortos.obj: ${APP_ROOT}/main_nortos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/trace.obj: ${APP_ROOT}/mcuboot_config/trace.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/flash_map_backend.obj: ${APP_ROOT}/flash_map_backend/flash_map_backend.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/ti_drivers_config.obj: ${PLATFORM}/ti_drivers_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/ti_devices_config.obj: ${PLATFORM}/ti_devices_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/boot_record.obj: ${GIT_ROOT}/boot/bootutil/src/boot_record.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/image_ed25519.obj: ${GIT_ROOT}/boot/bootutil/src/image_ed25519.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/bootutil_misc.obj: ${GIT_ROOT}/boot/bootutil/src/bootutil_misc.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/image_rsa.obj: ${GIT_ROOT}/boot/bootutil/src/image_rsa.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/image_validate.obj: ${GIT_ROOT}/boot/bootutil/src/image_validate.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/bootutil_public.obj: ${GIT_ROOT}/boot/bootutil/src/bootutil_public.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/loader.obj: ${GIT_ROOT}/boot/bootutil/src/loader.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/caps.obj: ${GIT_ROOT}/boot/bootutil/src/caps.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/swap_misc.obj: ${GIT_ROOT}/boot/bootutil/src/swap_misc.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/encrypted.obj: ${GIT_ROOT}/boot/bootutil/src/encrypted.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/swap_move.obj: ${GIT_ROOT}/boot/bootutil/src/swap_move.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/fault_injection_hardening.obj: ${GIT_ROOT}/boot/bootutil/src/fault_injection_hardening.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/fault_injection_hardening_delay_rng_mbedtls.obj: ${GIT_ROOT}/boot/bootutil/src/fault_injection_hardening_delay_rng_mbedtls.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/swap_scratch.obj: ${GIT_ROOT}/boot/bootutil/src/swap_scratch.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/image_ec.obj: ${GIT_ROOT}/boot/bootutil/src/image_ec.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/tlv.obj: ${GIT_ROOT}/boot/bootutil/src/tlv.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/image_ec256.obj: ${GIT_ROOT}/boot/bootutil/src/image_ec256.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@
	
build/obj/aes_decrypt.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/aes_decrypt.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/ccm_mode.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/ccm_mode.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/ctr_prng.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/ctr_prng.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/ecc_dsa.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/ecc_dsa.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/hmac_prng.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/hmac_prng.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/aes_encrypt.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/aes_encrypt.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/cmac_mode.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/cmac_mode.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/ecc.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/ecc.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/ecc_platform_specific.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/ecc_platform_specific.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/sha256.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/sha256.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/cbc_mode.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/cbc_mode.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/ctr_mode.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/ctr_mode.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/ecc_dh.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/ecc_dh.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/hmac.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/hmac.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/obj/utils.obj: ${GIT_ROOT}/ext/tinycrypt/lib/source/utils.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

build/$(NAME).out: $(OBJECTS)
	@ echo linking $@
	@ $(LNK)  $(OBJECTS)  $(LFLAGS) -o build/$(NAME).out

clean:
	@ echo Cleaning...
	@ rm -rf build
