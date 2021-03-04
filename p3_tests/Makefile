TESTFILES := $(wildcard *.crona)
TESTS := $(TESTFILES:.crona=.test)

.PHONY: all

all: $(TESTS)

%.test:
	@rm -f $*.unparse $*.err
	@touch $*.unparse $*.err
	@echo "TEST $*"
	@../cronac $*.crona -u $*.unparse 2> $*.err ;\
	PROG_EXIT_CODE=$$?;\
	if [ $$PROG_EXIT_CODE != 0 ]; then \
		echo "cronac error:"; \
		cat $*.err; \
		exit 1; \
	fi; \
	diff -B --ignore-all-space $*.unparse $*.unparse.expected; \
	STDOUT_DIFF_EXIT=$$?;\
	diff -B --ignore-all-space $*.err $*.err.expected; \
	STDERR_DIFF_EXIT=$$?;\
	FAIL=$$(($$STDOUT_DIFF_EXIT || $$STDERR_DIFF_EXIT));\
	exit $$FAIL

clean:
	rm -f *.unparse *.err
