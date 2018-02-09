# MPD
Maximizing optimal probability domain

The MPD program aims to compute the shape of the domain that maximizes the probability to find exactly a certain number of electrons inside it, given a chemical electronic structure.

In order to compile the program, you need to have previously installed:
* gcc (version 5.4.0)
* make (version 4.1)
* cmake (version 3.5.1)
* git (version 2.7.4)

and the libraries:
* liblapacke (3.6.0-2ubuntu2) package containing the Standard C language library for LAPACK (version 3.X).
* lib32gomp1 (5.4.0-6ubuntu1~16.04.6) which an implementation of OpenMP for the C language

In addition, a certain number of other iscd softwares (https://github.com/ISCDtoolbox) must have been previously installed
* medit to vizualize 3D mesh (see https://github.com/ISCDtoolbox/Medit)
* mmg3d_03 to adapt 3D mesh (see https://github.com/Mmgtools)
* mshdist to redistanciate level-set function (see https://github.com/ISCDtoolbox/Mshdist)
* elastic to solve elasticity on 3D mesh (see https://github.com/ISCDtoolbox/LinearElasticity)
* advect to advect a level-function function thanks to a vector field (see https://github.com/ISCDtoolbox/Advection)

Then, open your terminal and locate yourself at the right position to download the files thanks to "ls" and "cd" command
To clone the files, type

	git clone https://github.com/ISCDtoolbox/MPD.git

To enter the directory, type

	cd MPD

To create a build directory, type

	mkdir build

To enter the build directory, type

	cd build

To create the make file, type

	cmake ..

To compile the mpd program, type

	make

then if it is successfull, type

	make install

(Another more direct possibility is to go inside sources directory and type

	gcc -Wall main.c loadChemistry.c meshCube.c adaptCube.c shapeDerivative.c -lm -llapacke -fopenmp -o mpdProgram

to compile program without cmake and makefile)

Then, you can go inside bin directory by typing

	cd ../bin

and checks that mpdProgram has been created. In order to launch mpdProgram (cube.elas must exists in ../inputFiles/ directory) the line command syntax is the following

./mpdProgram chemicalNameFile.wfn numberOfElectronsToSearch (optionallyMeshNameFile)

In our case, as an example one can simply type

	./mpdProgram ../inputFiles/h2o-sto128g.wfn 1

and checks if it works





 





