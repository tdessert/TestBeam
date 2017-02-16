CMS HGCal Testbeam Analysis Framework
=============================================

### Content

- [Download the code](#download-the-code)
- [Location of data files](#location-of-data-files)
- [Running the code](#running-the-code)
- [Updates from the positionResolution branch](#updates-from-the-positionResolution-branch)
- [More information](#more-information)




## Download the code
* CMSMSW Version 8.0.1
* **`scram project ${RELEASE}`**
* **`cd ${RELEASE}/src/`**
* **`cmsenv`** 
* **`git cms-init`**
* **`git clone git@github.com:CMS-HGCAL/TestBeam.git HGCal/`**
* **`git checkout FNAL_TB_4Layers`**
* **`git pull`**
* **`scram b -j16`**

## Location of the data files

#### Raw data files
The RAW files can be found in **`/afs/cern.ch/user/r/rchatter/public/FNAL_TB_May/FNAL_TB_May_4Module_Runs`**.
This is structured by beam type and for the Electrons further by the energy of the electron beam.

#### Electronics map and pedestal files
* The EMAP for this run **`map_FNAL_Layer1234.txt`** can be found in CondObjects/data
The event average pedestal files for both low and high gain can be found in
**`Ped_LowGain_M1254.txt`** and **`Ped_HighGain_M1254.txt`** also in **`CondObjects/data`**.
* The information of the EMAP is needed by the framework when producing the digis from the 
RAW and is provided in the file **`RawToDigi/python/hgcaltbdigis_cfi.py`**.
It is also needed in analysis code if a conversion from Detector Id to Electronics Id is needed.
* May refer to the example **`RawToDigi/plugins/DigiPlotter.cc`** . In the **`EndJob()`** function of this same
code the format needed in which the pedestal files are to be written out can be found.
* The Pedestal information is needed in the step from Digis to Reco. And it is provided from
**`Reco/python/hgcaltbrechitproducer_cfi.py`**.

## Running the code
* In **`test_cfg.py`** the RAW data file to be run is to be provides in the line:

```
process.source = 
cms.Source("HGCalTBTextSource",
  run=cms.untracked.int32(1),
  fileNames=cms.untracked.vstring("file:FNAL_TB_May_4Module_Runs/PED/HGC_Output_1600.txt")
)
```

* In **`cms.Path`**: 
**`process.hgcaltbdigis`** produces digis and **`process.hgcaltbrechits`** produces the rechits. The analyzer to be run can come after this. There are a few provided:
**`RawToDigi/plugins/DigiPlotter.cc`** to generate the pedestals
**`Reco/plugins/RecHitPlotter_HighGain_Correlation_CM.cc`** for the noise studies starting from pedestal subtracted digis.

* The output root file name is set in:
```
process.TFileService = cms.Service("TFileService", fileName = cms.string("HGC_Output_1600_Reco.root") )
```

#### Event-based reconstruction (Outdated?)
```
sh scripts/rearrangeTxtFile.sh input.txt
>> CERN raw data rearranged in /eos/cms/store/group/upgrade/HGCAL/TestBeam/CERN/Sept2016/
cmsRun test_cfg_newEB.py chainSequence=6 pedestalsHighGain=CondObjects/data/Ped_HighGain_L8.txt pedestalsLowGain=CondObjects/data/Ped_LowGain_L8.txt runNumber=930 configuration=2 runType=HGCRun
>> chainSequence=<configuration>
>> test_cfg_newEB.py to load the reqarranged .txt:
-> mount eos
-> options.register('dataFolder','~/eos/cms/store/group/upgrade/HGCAL/TestBeam/CERN/Sept2016/',....
```

#### Overview of analysis configurations
* **`chainSequence=1`** Runs on Digis produces pedestal files in the path specifienfied under pedestalsHighGain and pedestalsLowGain
* **`chainSequence=4`** Runs event Display analyzer
* **`chainSequence=5`** Runs on Recos for each cell of the detector across layers. Use for pion, muon and pedestal run. Correlation across cells as implemented by Kai-Yu are evaluated.     
* **`chainSequence=6`** to run **`Layer_Sum_Analyzer`**: Evaluates for each layer as well as summed across layers: Max hit, Max hit + 6 nearest neighbours(7 cells), 19 cells and All cells --- **with each cell picked subject to a 2 MIP cut**. Results are available considering only the energy deposited in Silicon, and with dE/dx weights to recover the total energy deposited in the absorbers and the silicon sensors.

* **`configuration=-1`** ADCtoMIP for CERN (0 = ADCtoMIP for FNAL) 
* **`configuration=1`** (2) to select weights for 5X0 (25X0) 8 layers cern runs

## Updates from the positionResolution branch

### (Extended)TextInput Plugin
todo

### (New) GenSim to RecHit Converter
todo

### (New) PositionResolutionAnalyzer
todo

### (New) MillepedeBinaryWriter
todo

### Helper Classes
#### Geometry
todo

#### Sensors
todo

#### Tracks
todo

#### AlignmentParameters
todo

### Examplary run commands
todo


## More Information
For general instructions on the framework, co-ordinate system, reconstruction sequence, etc please refer to
[http://cms-hgcal.github.io/TestBeam/](http://cms-hgcal.github.io/TestBeam/)
