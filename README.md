# Enhancing Branch-and-Bound for Multi-Objective 0-1 Programming

This contains the C++ code for the branch-and-bound algorithm implemented in the paper N. Forget, S.N. Parragh, "Enhancing Branch-and-bound for Multi-Objective 0-1 Programming", INFORMS Journal on Computing, 2023. Below are detailed the different parameters and the configurations used for each section of the experiments. All files where compiled using Visual Studio 2022 on Windows. The main function is contained in project2.cpp.

## Parameters

Here are detailed the various parameters and corresponding values used in the algorithm.

* Parameter 1: path to the data file of the instance

* Parameter 2: lower bound set computation method
..* 101: linear relaxation computed when the node is selected
..* 102: linear relaxation computed when the node is created

* Parameter 3: node selection strategy
..* 200: depth first
..* 201: breadth first
..* 204: Minimal weighted-sum value
..* 207: Minimum gap

* Parameter 4: probing
..* 301: probing disabled
..* 306: probing enabled

* Parameter 5: objective branching
..* 400: no objective branching
..* 401: cone bounding
..* 402: full objective branching

* Parameter 6: time limit, expressed in seconds

* Parameter 7: dominance test with the weigthed-sum during probing (only relevant when Parameter 4 is 306, use 0 otherwise)
..* 0: disabled
..* 2: enabled

* Parameter 8: scale the objective function in the weighted-sum value (only relevant when Parameter 3 is 204, use 0 otherwise)
..* 0: disabled
..* 1: enabled

* Parameter 9: cover cuts
..* 0: disabled
..* 1: enabled

* Parameter 10: variable selection rule for Facility Location Problems (only relevant when the problem class is UFLP or CFLP, use 0 otherwise)
..* 0: disabled
..* 1: enabled

* Parameter 11: enumeration threshold. Should be a positive integer.

## Test configurations

Here are detailed the test configurations for each section of the paper.

### 7.1 - Probing and objective branching

The following configurations are tested / parameters are compared.

* For UFLP & CFLP:
..* NVF - NOB: instance 101 201 301 400 3600 0 0 0 0 0
..* NVF - CB : instance 101 201 301 401 3600 0 0 0 0 0
..* NVF - FOB: instance 101 201 301 402 3600 0 0 0 0 0
..* VF - NOB : instance 101 201 306 400 3600 0 0 0 0 0
..* VF - CB  : instance 101 201 306 401 3600 0 0 0 0 0
..* VF - FOB : instance 101 201 306 402 3600 0 0 0 0 0

* For KP:
..* NVF - NOB: instance 101 200 301 400 3600 0 0 0 0 0
..* NVF - CB : instance 101 200 301 401 3600 0 0 0 0 0
..* NVF - FOB: instance 101 200 301 402 3600 0 0 0 0 0
..* VF - NOB : instance 101 200 306 400 3600 0 0 0 0 0
..* VF - CB  : instance 101 200 306 401 3600 0 0 0 0 0
..* VF - FOB : instance 101 200 306 402 3600 0 0 0 0 0

### 7.2 - Combining probing and bounding

The following configurations are tested / parameters are compared.

* For UFLP & CFLP:
..* FOB - VF  : instance 101 201 306 402 3600 2 0 0 0 0
..* FOB - VFD : instance 101 201 306 402 3600 2 0 0 0 0

* For KP:
..* FOB - VF  : instance 101 200 306 402 3600 2 0 0 0 0
..* FOB - VFD : instance 101 200 306 402 3600 2 0 0 0 0

### 7.3 - Node selection rules

The following configurations are tested / parameters are compared.

* For UFLP & CFLP:
..* DB : instance 101 201 306 402 3600 2 0 0 0 0

* For KP:
..* DB : instance 101 200 306 402 3600 2 0 0 0 0

* For all:
..* BBWS  : instance 103 204 306 402 3600 2 0 0 0 0
..* BBWSN : instance 103 204 306 402 3600 2 1 0 0 0
..* BBGAP : instance 103 207 306 402 3600 2 0 0 0 0

### 7.4 - Objective branching and cover cuts

The following configurations are tested / parameters are compared.

* For UFLP:
..* NCC : instance 103 204 306 402 3600 2 1 0 0 0
..* CC  : instance 103 204 306 402 3600 2 1 1 0 0

### 7.5 - Enumeration in MOBB

The following configurations are tested / parameters are compared.

* For all:
..* BB   : instance 103 204 306 402 3600 2 1 0 0 0
..* BB-E : instance 103 204 306 402 3600 2 1 0 0 14

### 7.6 - Problem specific variable selection rules: the case of CFLP

The following configurations are tested / parameters are compared.

* For CFLP:
..* MOF : instance 103 204 306 402 3600 2 1 0 0 14
..* PS  : instance 103 204 306 402 3600 2 1 0 1 14

### 7.7 - Probing in the integer case

The following configurations are tested / parameters are compared.

* For ILP:
..* BB-int-NVF : instance 103 204 301 402 3600 2 1 0 0 14
..* BB-int-VF  : instance 103 204 306 402 3600 2 1 0 0 14

### 7.8 - Comparison with objective space search algorithms

The following configurations are tested / parameters are compared.

* For all:
..* BB-E : instance 103 204 306 402 3600 2 1 0 0 14

* For UFLP
..* BB-E : instance 103 204 306 401 3600 2 1 0 0 14