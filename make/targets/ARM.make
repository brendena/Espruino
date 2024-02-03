$(PROJ_NAME).elf: $(OBJS) $(LINKER_FILE)
	@echo $($(quiet_)link)
	@$(call link)

$(PROJ_NAME).lst : $(PROJ_NAME).elf
	@echo $($(quiet_)obj_dump)
	@$(call obj_dump)

$(PROJ_NAME).srec : $(PROJ_NAME).elf
	@echo $(call $(quiet_)obj_to_bin,srec,srec)
	@$(call obj_to_bin,srec,srec)

$(PROJ_NAME).bin : $(PROJ_NAME).elf
	@echo $(call $(quiet_)obj_to_bin,binary,bin)
	@$(call obj_to_bin,binary,bin)
	bash scripts/check_size.sh $(PROJ_NAME).bin
ifdef PAD_FOR_BOOTLOADER
	@echo Padding $(PROJ_NAME).bin
	mv $(PROJ_NAME).bin $(PROJ_NAME).bin.unpadded
	tr "\000" "\377" < /dev/zero | dd bs=1 count=$(shell python scripts/get_board_info.py $(BOARD) "common.get_espruino_binary_address(board)") of=$(PROJ_NAME).bin
	cat $(PROJ_NAME).bin.unpadded >> $(PROJ_NAME).bin
endif

gdb:
	@echo "target extended-remote :3333" > gdbinit
	@echo "file $(PROJ_NAME).elf" >> gdbinit
	@echo "load bin/espruino_2v19.88_p8_SDK12_SD30_SPIFLASH.hex" >> gdbinit
	@echo "load bin/espruino_2v19.88_p8_SDK12_SD30_SPIFLASH.elf" >> gdbinit
#	@echo "break graphicsFillRect" >> gdbinit
#	@echo "break ./libs/banglejs/jswrap_bangle.c:1528" >> gdbinit
	@echo "break HardFault_Handler" >> gdbinit
#	@echo "break flush_chunk_buffer" >> gdbinit
	
	@echo $(GDB)
#	$(GDB) -x gdbinit
	./gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-gdb -x gdbinit
