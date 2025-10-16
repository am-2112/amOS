efiinc = ..////include//efi

efilibs = libs

geninc = ..//..////general//include

genlibs = ../..////general//libs

interfaces = ..////include//interfaces

allincludes = -I$(efiinc) -I$(efilibs) -I$(geninc) -I$(genlibs) -I$(interfaces)

efidependencies := $(wildcard $(efilibs)//*.c)

gendependencies := $(wildcard $(genlibs)//*.c)

efiobjects := $(patsubst $(efilibs)//%.c,build//%.o,$(efidependencies))

genobjects := $(patsubst $(genlibs)//%.c,build//%.o,$(gendependencies))

$(BUILD_PATH)//BOOTX64.EFI: build//entry.o $(efiobjects) $(genobjects)
	$(GCC_PATH)//x86_64-w64-mingw32-gcc -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main -o $(BUILD_PATH)//BOOTX64.EFI build//*.o

build//entry.o: entry.c
	$(GCC_PATH)//x86_64-w64-mingw32-gcc -ffreestanding $(allincludes) -c -o "build//entry.o" entry.c

$(efiobjects): $(efidependencies)
	$(GCC_PATH)//x86_64-w64-mingw32-gcc -ffreestanding $(allincludes) -c $(dir $<)$(subst .o,.c,$(@F)) -o $(@)

$(genobjects): $(gendependencies)
	$(GCC_PATH)//x86_64-w64-mingw32-gcc -ffreestanding $(allincludes) -c $(dir $<)$(subst .o,.c,$(@F)) -o $(@)