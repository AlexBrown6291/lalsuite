BUILT_SOURCES =
include $(top_srcdir)/gnuscripts/lalapps.am

bin_PROGRAMS = \
	lalapps_SemiAnalyticF \
	lalapps_compareFstats\
	lalapps_PredictFstat \
	lalapps_ComputeFstatistic_v2 \
	lalapps_ComputeFstatMCUpperLimit \
	lalapps_synthesizeBstatMC \
	lalapps_synthesizeTransientStats \
	lalapps_synthesizeLVStats

EXTRA_PROGRAMS = \
	lalapps_CFSv3 \
	eah_PredictFstat \
	eah_ComputeFstatistic_v2

lalapps_SemiAnalyticF_SOURCES = SemiAnalyticF.c

lalapps_compareFstats_SOURCES = compareFstats.c

ComputeFstatisticV2Sources = \
	ComputeFstatistic_v2.c \
	HeapToplist.c \
	HeapToplist.h

lalapps_ComputeFstatistic_v2_SOURCES = $(ComputeFstatisticV2Sources)

lalapps_PredictFstat_SOURCES = PredictFstat.c

# force linking in C++ mode for E@H
eah_PredictFstat_SOURCES = \
	$(lalapps_PredictFstat_SOURCES) \
	../EinsteinAtHome/win_lib.cpp
eah_ComputeFstatistic_v2_SOURCES = \
	$(ComputeFstatisticV2Sources) \
	../EinsteinAtHome/win_lib.cpp

lalapps_ComputeFstatMCUpperLimit_SOURCES = ComputeFstatMCUpperLimit.c

lalapps_synthesizeBstatMC_SOURCES = synthesizeBstatMC.c

lalapps_synthesizeTransientStats_SOURCES = synthesizeTransientStats.c

lalapps_synthesizeLVStats_SOURCES = synthesizeLVStats.c

lalapps_CFSv3_SOURCES = CFSv3.c

EXTRA_DIST = \
	testPredictFstat.sh \
	testCFSv2.sh \
	testCFSv2_grids.sh \
	testCFSv2_resamp.sh \
	testCFSv2_binary.sh \
	testSynthLV.sh

EXTRA_DIST += \
	testCFSv2.dat.ref.gz \
	testCFSv2_grid0.dat.ref.gz \
	testCFSv2_grid1.dat.ref.gz \
	testCFSv2_grid2.dat.ref.gz \
	testCFSv2_grid3.dat.ref.gz \
	testCFSv2_grid6.dat.ref.gz \
	testCFSv2_grid8.dat.ref.gz \
	testCFSv2_grid9.dat.ref.gz

TESTS = testPredictFstat.sh \
	testSynthLV.sh

if !CONDOR_ENABLED
TESTS += \
	testCFSv2.sh \
	testCFSv2_grids.sh \
	testCFSv2_resamp.sh \
	testCFSv2_binary.sh
endif

TESTS_ENVIRONMENT = \
	LAL_DEBUG_LEVEL=msglvl1,memdbg; export LAL_DEBUG_LEVEL; \
	LAL_DATA_PATH="$(LAL_DATA_PATH)"; export LAL_DATA_PATH;