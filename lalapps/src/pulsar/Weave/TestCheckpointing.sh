# Perform an interpolating search without/with checkpointing, and check for consistent results

echo "=== Create search setup with 3 segments ==="
set -x
${builddir}/lalapps_WeaveSetup --first-segment=1122332211/90000 --segment-count=3 --detectors=H1,L1 --output-file=WeaveSetup.fits
set +x
echo

echo "=== Generate SFTs ==="
set -x
${injdir}/lalapps_Makefakedata_v5 --randSeed=3456 --fmin=49.5 --Band=2.0 --Tsft=1800 \
    --outSingleSFT --outSFTdir=. --IFOs=H1,L1 --sqrtSX=1,1 \
    --timestampsFiles=${srcdir}/timestamps-irregular.txt,${srcdir}/timestamps-regular.txt
set +x
echo

echo "=== Perform interpolating search without checkpointing ==="
set -x
${builddir}/lalapps_Weave --output-file=WeaveOutNoCkpt.fits \
    --output-toplist-limit=5000 --output-per-detector --output-per-segment --setup-file=WeaveSetup.fits --sft-files='*.sft' \
    --sky-patch-count=3 --sky-patch-index=1 --freq=50/1e-4 --f1dot=-1e-9,0 --semi-max-mismatch=5 --coh-max-mismatch=0.4
set +x
echo

echo "=== Check average number of semicoherent templates per dimension is more than one"
set -x
for dim in SSKYA SSKYB NU0DOT NU1DOT; do
    ${fitsdir}/lalapps_fits_header_getval "WeaveOutNoCkpt.fits[0]" "SEMIAVG ${dim}" > tmp
    semi_avg_ntmpl_dim=`cat tmp | xargs printf "%d"`
    expr ${semi_avg_ntmpl_dim} '>' 1
done
set +x
echo

echo "=== Perform interpolating search with checkpointing ==="
echo "--- Start to first checkpoint ---"
set -x
rm -f WeaveCkpt.fits
${builddir}/lalapps_Weave --output-file=WeaveOutCkpt.fits --ckpt-output-file=WeaveCkpt.fits --ckpt-output-exit=0.22 \
    --output-toplist-limit=5000 --output-per-detector --output-per-segment --setup-file=WeaveSetup.fits --sft-files='*.sft' \
    --sky-patch-count=3 --sky-patch-index=1 --freq=50/1e-4 --f1dot=-1e-9,0 --semi-max-mismatch=5 --coh-max-mismatch=0.4
set +x
echo "--- First to second checkpoint ---"
set -x
${builddir}/lalapps_Weave --output-file=WeaveOutCkpt.fits --ckpt-output-file=WeaveCkpt.fits --ckpt-output-exit=0.63 \
    --output-toplist-limit=5000 --output-per-detector --output-per-segment --setup-file=WeaveSetup.fits --sft-files='*.sft' \
    --sky-patch-count=3 --sky-patch-index=1 --freq=50/1e-4 --f1dot=-1e-9,0 --semi-max-mismatch=5 --coh-max-mismatch=0.4
set +x
echo "--- Second checkpoint to end ---"
set -x
${builddir}/lalapps_Weave --output-file=WeaveOutCkpt.fits --ckpt-output-file=WeaveCkpt.fits \
    --output-toplist-limit=5000 --output-per-detector --output-per-segment --setup-file=WeaveSetup.fits --sft-files='*.sft' \
    --sky-patch-count=3 --sky-patch-index=1 --freq=50/1e-4 --f1dot=-1e-9,0 --semi-max-mismatch=5 --coh-max-mismatch=0.4
set +x
echo

echo "=== Check number of times output results have been restored from a checkpoint ==="
set -x
${fitsdir}/lalapps_fits_header_getval "WeaveCkpt.fits[0]" CKPTCNT > tmp
ckpt_count=`cat tmp | xargs printf "%d"`
expr ${ckpt_count} '=' 2
${fitsdir}/lalapps_fits_header_getval "WeaveOutCkpt.fits[0]" NUMCKPT > tmp
num_ckpt=`cat tmp | xargs printf "%d"`
expr ${num_ckpt} '=' ${ckpt_count}
set +x
echo

echo "=== Compare F-statistics from lalapps_Weave without/with checkpointing ==="
set -x
LAL_DEBUG_LEVEL="${LAL_DEBUG_LEVEL},info"
${builddir}/lalapps_WeaveCompare --setup-file=WeaveSetup.fits --output-file-1=WeaveOutNoCkpt.fits --output-file-2=WeaveOutCkpt.fits
set +x
echo
