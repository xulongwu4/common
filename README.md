# Set up for converting GMp raw data to ROOT files

This repo contains macros, scripts, and database files used to convert GMp raw data to ROOT files.

After cloning this repo, one still needs to do the following before proceeding to replay a GMp run:
1. Create directories *rootfiles* and *summaryfiles* in the replay directory:
```
mkdir -p replay/rootfiles replay/summaryfiles
```
2. To submit batch jobs to JLab's scientific computing farm, one needs to have a *jobs* folder located in the *replay/batch* directory:
```
mkdir -p replay/batch/jobs
```
3. The code in *replay/ReplayCore64.C* needs to be compiled by [**Hall A analyzer**](https://github.com/JeffersonLab/analyzer) by runing the following command from the command line:
```
cd replay
analyzer # Start the Hall A analyzer
.L ReplayCore64.C+
.q # Quit the Hall A analyzer
```
4. To have relevant libraries compiled, just go to corresponding directories in *libraries*, and run `make` from the command line. For example, to compile the library *libGmp_Beam_Eloss.so*, one can do:
```
cd libraries/Gmp_Beam_Eloss
make
```
5. Once the previous steps were completed with no errors, one can start replaying a GMp run by providing a run number to the script *replay\_L.C* (for left HRS runs) or *replay\_R.C* (for right HRS runs):
```
analyzer
.x replay_L.C(13831)
```
6. To submit batch jobs to JLab's computing farm, use the *gmp_replay_manager* bash script in the *replay/batch* directory.
```
# Suppose gmp_replay_manager is in the PATH
gmp_replay_manager 13831 # replay a single run
gmp_replay_manager K3-4 # replay a whole kinematics
``` 
