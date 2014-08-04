.PHONY: scoreur all clean ver98
export vers = c++0x
### Please change according to the version of your compiler ###

VPATH = src
objects = main.o wav.o score.o complex.o spectrum.o
scoreur: #$(objects) #use makefile in subdir ./src instead
	@cd src && make -e
ver98:
	@cd src && make

########## This is comment ##########
all:
	@cd src && make -e all
clean:
	@-rm scoreur
	@cd src && make clean
