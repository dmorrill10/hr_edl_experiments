FRCFR =f-rcfr
EXE :=$(FRCFR)/build/bin/run_cfr

default: results/Error.pdf results/Plateau.pdf results/Exploitability_Num_Part.pdf
	@true

SEEDS_LEDUC :=$(shell seq 1 5)
NUM_PARTITIONS_LEDUC :=10 20 30 40 50
ALGS_LEDUC :=hedge-t0_01 hedge-t0_05 hedge-t0_1 hedge-t0_5 hedge-t1 \
 poly-p1_1 poly-p1_5 poly-p2_5 poly-p3 rm

NUM_PARTITIONS_GOOFSPIEL :=20 40 50 60
ALGS_GOOFSPIEL :=hedge-t0_1 hedge-t0_5 hedge-t1 hedge-t5 hedge-t10 \
	poly-p1_1 poly-p1_5 poly-p2_5 poly-p3 rm

NUM_PARTITIONS_RANDOM_GOOFSPIEL :=30 60 90 120
ALGS_RANDOM_GOOFSPIEL :=hedge-t0_01 hedge-t0_05 hedge-t0_1 hedge-t0_5 hedge-t1 \
	poly-p1_1 poly-p1_5 poly-p2_5 poly-p3 rm

data:
	mkdir data

results:
	mkdir results

results/Error.pdf: \
			data/goofspiel.null.rm.gen.txt \
			$(foreach i,$(SEEDS_LEDUC),\
			$(foreach n,$(NUM_PARTITIONS_GOOFSPIEL),\
			$(foreach alg,$(ALGS_GOOFSPIEL),data/goofspiel.null.$(alg)-s10-n$(n).$(i).gen.txt))) \
			data/leduc.null.rm.gen.txt \
			$(foreach i,$(SEEDS_LEDUC),\
			$(foreach n,$(NUM_PARTITIONS_LEDUC),\
			$(foreach alg,$(ALGS_LEDUC),data/leduc.null.$(alg)-s10-n$(n).$(i).gen.txt))) \
			data/random_goofspiel.null.rm.gen.txt \
			$(foreach i,$(SEEDS_LEDUC),\
			$(foreach n,$(NUM_PARTITIONS_RANDOM_GOOFSPIEL),\
			$(foreach alg,$(ALGS_RANDOM_GOOFSPIEL),data/random_goofspiel.null.$(alg)-s10-n$(n).$(i).gen.txt))) | results
	python3 bin/plot_aamas_figures.py

results/Exploitability_Num_Part.pdf: \
		data/goofspiel.null.rm.gen.txt \
		$(foreach i,$(SEEDS_LEDUC),\
		$(foreach n,$(NUM_PARTITIONS_GOOFSPIEL),\
		$(foreach alg,$(ALGS_GOOFSPIEL),data/goofspiel.null.$(alg)-s10-n$(n).$(i).gen.txt))) \
		data/leduc.null.rm.gen.txt \
		$(foreach i,$(SEEDS_LEDUC),\
		$(foreach n,$(NUM_PARTITIONS_LEDUC),\
		$(foreach alg,$(ALGS_LEDUC),data/leduc.null.$(alg)-s10-n$(n).$(i).gen.txt))) \
		data/random_goofspiel.null.rm.gen.txt \
		$(foreach i,$(SEEDS_LEDUC),\
		$(foreach n,$(NUM_PARTITIONS_RANDOM_GOOFSPIEL),\
		$(foreach alg,$(ALGS_RANDOM_GOOFSPIEL),data/random_goofspiel.null.$(alg)-s10-n$(n).$(i).gen.txt))) | results
	python3 bin/plot_aamas_figures.py

results/Plateau.pdf: \
		data/goofspiel.null.rm.gen.txt \
		$(foreach i,$(SEEDS_LEDUC),\
		$(foreach n,$(NUM_PARTITIONS_GOOFSPIEL),\
		$(foreach alg,$(ALGS_GOOFSPIEL),data/goofspiel.null.$(alg)-s10-n$(n).$(i).gen.txt))) \
		data/leduc.null.rm.gen.txt \
		$(foreach i,$(SEEDS_LEDUC),\
		$(foreach n,$(NUM_PARTITIONS_LEDUC),\
		$(foreach alg,$(ALGS_LEDUC),data/leduc.null.$(alg)-s10-n$(n).$(i).gen.txt))) \
		data/random_goofspiel.null.rm.gen.txt \
		$(foreach i,$(SEEDS_LEDUC),\
		$(foreach n,$(NUM_PARTITIONS_RANDOM_GOOFSPIEL),\
		$(foreach alg,$(ALGS_RANDOM_GOOFSPIEL),data/random_goofspiel.null.$(alg)-s10-n$(n).$(i).gen.txt))) | results
	python3 bin/plot_aamas_figures.py


.PHONY: aamas

aamas: results/Error.pdf results/Plateau.pdf results/Exploitability_Num_Part.pdf

$(EXE):
	cd $(FRCFR) && $(MAKE)

data/%.gen.txt: | data
	python3 bin/run_experiment.py --exe $(EXE) -a $* > $@

runs_remaining.gen.sh: bin/list_runs_remaining.sh Makefile
	$< > $@

print-%:
	@echo $* = $($*)
