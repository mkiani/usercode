# usercode

TO REPRODUCE THE MASS FITS IN DIFFERENT SAMPLES/CATEGORIES

A.The command line Signal Resonace Shape with no bkg(tail part)

 1.  root -l
 2.  .x lib.C
 3.  .L fitSignalShape.C++
 4.  all(0,0,1)

The order of the Arguments of all(channels,Category, Sample) channels(0=4mu,1=4e,2=2e2mu) Category(0=Untagged, 1=1-jet tagged 2=VBF tagged 3=VH-leptonic tagged 4=VH-hadronic tagged 5=ttH tagged) Sample(1=ggH , 2=VBFH125 , 3= WplusH125, 4=WminusH125, 5=ttH125 )

B.The command line Signal Resonace Shape with Chebyshev bkg(tail part)

 1.  root -l
 2.  .x lib.C
 3.  .L fitSignalContinuumShape.C++
 4.  all(0,0,1)

The order of the Arguments of all(channels,Category, Sample) channels(0=4mu,1=4e,2=2e2mu) Category(0=Untagged, 1=1-jet tagged 2=VBF tagged 3=VH-leptonic tagged 4=VH-hadronic tagged 5=ttH tagged) Sample(1=ZH125 , 2= WplusH125, 3=WminusH125, 4=ttH125)

Note: You have to run fitSignalShape.C before fitSignalContinuumShape.C for the resonance parameters. 
