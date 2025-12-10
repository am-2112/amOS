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

$(BUILD_PATH)//efi//boot//BOOTX64.EFI: build//entry.o $(efiobjects) $(genobjects)
	x86_64-w64-mingw32-gcc -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main -o $(BUILD_PATH)//efi//boot////BOOTX64.EFI build//*.o

build//entry.o: entry.c
	x86_64-w64-mingw32-gcc -ffreestanding $(allincludes) -c -o "build//entry.o" entry.c

$(efiobjects): $(efidependencies)
	x86_64-w64-mingw32-gcc -ffreestanding $(allincludes) -c $(dir $<)$(subst .o,.c,$(@F)) -o $(@)

$(genobjects): $(gendependencies)
	x86_64-w64-mingw32-gcc -ffreestanding $(allincludes) -c $(dir $<)$(subst .o,.c,$(@F)) -o $(@)