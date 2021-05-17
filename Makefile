##
## EPITECH PROJECT, 2021
## plazza
## File description:
## Makefile
##

CXX			?=	g++

RM			?=	rm -f

ECHO		=	`which echo`

MAKEFLAGS	+=	--no-print-directory

LDLIBS		=	-lpthread

CXXFLAGS	+=	-std=c++17 -Wall -Wextra -Weffc++

CPPFLAGS	=	-iquote include/

TFLAGS		=	-lcriterion --coverage

NAME		=	plazza

SRC			=	src/core.cpp	\
				src/kitchen.cpp	\
				src/shell.cpp	\

MAIN		=	src/main.cpp

MOBJ		=	$(MAIN:.cpp=.o)

OBJ			=	$(SRC:.cpp=.o)

TSRC		=	tests/test.cpp

TOBJ		=	$(TSRC:.cpp=.o)

GCDA		+=	$(SRC:.cpp=.gcda)
GCDA		+=	$(TSRC:.cpp=.gcda)

GCNO		+=	$(SRC:.cpp=.gcno)
GCNO		+=	$(TSRC:.cpp=.gcno)

ifdef DEBUG
	CXXFLAGS += -ggdb3
endif

ifneq (,$(findstring xterm,${TERM}))
GREEN       := $(shell tput -Txterm setaf 2)
RED         := $(shell tput -Txterm setaf 1)
RESET 		:= $(shell tput -Txterm sgr0)
else
GREEN       := ""
RED         := ""
RESET       := ""
endif

%.o:	%.cpp
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $< && \
	($(ECHO) "${GREEN}[ OK ]${RESET}" $<) || \
	($(ECHO) "${RED}[ NO COMPIL ]${RESET}" $<; false)

all:	$(NAME)

$(NAME):	$(OBJ) $(MOBJ)
	@$(CXX) $(MOBJ) $(OBJ) -o $(NAME) $(LDLIBS)

tests_run:	CPPFLAGS	+=	--coverage

tests_run:	$(OBJ) $(TOBJ)
	@$(CXX) -o unit_test $(OBJ) $(TOBJ) $(TFLAGS)
	@./unit_test
	@gcovr -b --exclude-directories tests

clean:
	@for f in $(TOBJ); do if [ -f $$f ]; then $(ECHO) "${GREEN}[ TU ]${RESET}" $$f; fi; done;
	@for f in $(OBJ); do if [ -f $$f ]; then $(ECHO) "${RED}[ RM ]${RESET}" $$f; fi; done;
	@for f in $(MOBJ); do if [ -f $$f ]; then $(ECHO) "${RED}[ RM ]${RESET}" $$f; fi; done;
	@$(RM) $(OBJ) $(MOBJ) $(TOBJ)
	@$(RM) $(GCDA)
	@$(RM) $(GCNO)

fclean: clean
	@if [ -f $(NAME) ]; then $(ECHO) "${RED}[ RM ]${RESET}" $(NAME); fi;
	@$(RM) $(NAME) unit_test

re: fclean all

.PHONY: all clean fclean re tests_run