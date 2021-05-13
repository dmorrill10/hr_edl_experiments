EXPERIMENT_RUNNER_DIRECTORY =hr_edl
EXE_DIR :=$(EXPERIMENT_RUNNER_DIRECTORY)/build.optimized/bin
SIF :=

ALG_GROUPS :=efr
GAMES :=sheriff tiny_bridge leduc tiny_hanabi \
	goofspiel goofspiel_ascending random_goofspiel goofspiel_3p goofspiel_ascending_3p
DET_MODES :=fixed sim
DET_SSV_FILES :=$(foreach alg_group,$(ALG_GROUPS),\
	$(foreach game,$(GAMES),\
		$(foreach mode,$(DET_MODES),\
			data/$(alg_group).$(game).null.$(mode).gen.ssv)))
SSV_FILES :=$(DET_SSV_FILES)

default: results/efr_data.npy results/cor_gap_data.npy
	@true

data:
	mkdir $@

results:
	mkdir $@

data/%.gen.ssv: | data
	python3 bin/run_experiment.py --exe_dir $(EXE_DIR) -a $* --sif $(SIF) > $@

runs_remaining.gen.sh: bin/list_runs_remaining.sh Makefile
	$< > $@

results/efr_data.npy: $(SSV_FILES) | results
	python3 bin/save_data.py -x efr -o $@

results/cor_gap_data.npy: $(wildcard data/cor_gap.*.dat) | results
	python3 bin/save_data.py -x cor_gap -o $@

print-%:
	@echo $* = $($*)
