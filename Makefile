# use bash for `time`
SHELL:=/bin/bash

# build output dir
BUILD:=build

# all targets
TARGETS:=stop_wait gbn sr tcp

all-tests:=$(addprefix test-, $(TARGETS))
all-targets:=$(addprefix $(BUILD)/, $(TARGETS))

all: $(all-targets) $(all-test)

clean: $(BUILD)
	$(MAKE) -C $<

test: $(all-tests)

$(BUILD):
	mkdir $@
	cmake -H. -B$@

$(BUILD)/%: $(BUILD)
	$(MAKE) -C $< $*

test-%: $(BUILD)/%
	time ./check_linux.sh $<
