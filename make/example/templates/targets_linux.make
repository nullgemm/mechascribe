bin/$(NAME): $(OBJ) $(OBJ_EXTRA)
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

bin/noto.bin.zst: res/noto_pack
	cp res/noto_pack/noto.bin.zst bin/noto.bin.zst

leak: bin/$(NAME) bin/noto.bin.zst
	cd bin && valgrind $(VALGRIND) 2> ../valgrind.log $(CMD)
	less valgrind.log

run: bin/$(NAME) bin/noto.bin.zst
	cd bin && $(CMD)
