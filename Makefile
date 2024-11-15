# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aschenk <aschenk@student.42berlin.de>      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/07 16:20:40 by aschenk           #+#    #+#              #
#    Updated: 2024/11/15 17:20:02 by aschenk          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME :=			miniRT

OS := 			$(shell uname)			# Detect OS type

#############################
# PRE-COMPILATION CONSTANTS #
#############################

WINDOW_W ?=		1440 					# Default window width
WINDOW_H ?=		900						# Default window height

#########################
# SOURCE & HEADER FILES #
#########################

SRCS_DIR :=		src
SRCS :=			$(SRCS_DIR)/main.c \
				$(SRCS_DIR)/initialization.c \
				$(SRCS_DIR)/event_handling.c \
				$(SRCS_DIR)/utils/mlx_utils.c \
				$(SRCS_DIR)/utils/error_exit.c \
				$(SRCS_DIR)/utils/cleanup.c

OBJS_DIR :=		obj
OBJS :=			$(SRCS:$(SRCS_DIR)/%.c=$(OBJS_DIR)/%.o)		# Each o. file has a corresponding c. file

HDRS_DIR :=		include
HDRS := 		$(HDRS_DIR)/main.h \
				$(HDRS_DIR)/colors.h \
				$(HDRS_DIR)/errors.h \
				$(HDRS_DIR)/types.h

#############
# LIBRARIES #
#############

# LIBFT
LIBFT_DIR :=	lib/libft
LIBFT_FLAGS :=	-L$(LIBFT_DIR) -lft		# -L: path to look for library; -l: library name
LIBFT :=		$(LIBFT_DIR)/libft.a

# MiniLibX
ifeq ($(strip $(OS)),Linux)				# Choose correct MiniLibX library based on OS
	MLX_LIB :=	mlx_Linux
else ifeq ($(strip $(OS)),Darwin)
	MLX_LIB :=	mlx_Darwin
else
	MLX_LIB :=	mlx
endif

MLX_DIR :=		lib/mlx
MLX_FLAGS :=	-L$(MLX_DIR) -l$(MLX_LIB) -lXext -lX11 -lm	# -L: path to look for library; -l: library name
LIBMLX := 		$(MLX_DIR)/lib$(MLX_LIB).a

LIB_FLAGS :=	$(LIBFT_FLAGS) $(MLX_FLAGS)

#####################
# COMPILATION RULES #
#####################

CC :=			cc
CFLAGS :=		-Wall -Wextra -Werror
CFLAGS +=		-I$(HDRS_DIR) -I$(LIBFT_DIR) -I$(MLX_DIR)		# Look for headers in these directories
CFLAGS +=		-DWINDOW_H=$(WINDOW_H) -DWINDOW_W=$(WINDOW_W)	# Define window dimensions with pre-compilation constants

CFLAGS +=		-g -Wpedantic						# Debugging flag, pedantic warnings

ifeq ($(strip $(OS)),Darwin)						# Suppress some errors/warnings on MacOS (due to the way prototypes are defined in MiniLibX)
	CFLAGS += 		-Wno-strict-prototypes
endif

######################
# FORMATTING STRINGS #
######################

RESET :=		\033[0m
BOLD :=			\033[1m
RED :=			\033[91m
GREEN :=		\033[32m
YELLOW :=		\033[33m

##############################################
# COMPILATION OF OBJECT FILES / PROGRESS BAR #
##############################################

TOTAL_SRCS :=	$(words $(SRCS))	# Total number of source files
SRC_COMP :=		0					# Number of source files compiled (object files created)

# Rule to define how to generate object files (%.o) from corresponding
# source files (%.c). Each .o file depends on the associated .c file and the
# project header files.
$(OBJS_DIR)/%.o:	$(SRCS_DIR)/%.c $(HDRS)		# Following lines are executed for each .o file
	@mkdir -p $(@D)		# create directory if it doesn't exist; -p: no error if existing, @D: directory part of target

# Update progress bar variables
	@$(eval SRC_COMP := $(shell expr $(SRC_COMP) + 1))							# Increment SRC_NUM
	@$(eval PERCENT := $(shell echo "$(SRC_COMP) / $(TOTAL_SRCS) * 100" | bc))	# Calculate percentage
	@$(eval PROGRESS := $(shell expr $(PERCENT) / 5))							# Calculate progress in 5% steps

# Print progress bar
	@printf "$(BOLD)\rCompiling $(NAME): ["										# '\r' moves cursor to beginning of line, overwriting previous line -> progess shown on same line
	@printf "$(GREEN)%0.s#$(RESET)$(BOLD)" $(shell seq 1 $(PROGRESS))			# Print one '#' for each 5% progress
	@if [ $(PERCENT) -lt 100 ]; then printf "%0.s-" $(shell seq 1 $(shell expr 20 - $(PROGRESS))); fi	# Prints '-' for remaining progress (not filled with '#')
	@printf "] "
	@if [ $(PERCENT) -eq 100 ]; then printf "$(GREEN)"; fi						# Switch to green color for 100%
	@printf "%d/%d - " $(SRC_COMP) $(TOTAL_SRCS)									# Print current compiled and total number of files
	@printf "%d%% $(RESET)" $(PERCENT)											# Print percentage

# Compile source file into object file
# - '-c':		Generates o. files without linking.
# - '$<':		First dependency; '$<' is replaced with the first dependency (the c. file).
# - '-o $@':	Output file name;  '$@' is replaced with target name (the o. file).
	@$(CC) $(CFLAGS) -c $< -o $@

#############################
# 'ALL' RULE WITH DEPENCIES #
#############################

# Building dependencies MiniLibX, libft, and the program when 'make' is called.
all:	$(LIBMLX) $(LIBFT) $(NAME)

# Compiling MiniLibX. Clones from official repo if not present.
# Output of cloning / compiliation supressed via redirecting '>/dev/null 2>&1'.
$(LIBMLX):
	@if [ ! -d "$(MLX_DIR)" ]; then \
		echo "Cloning MiniLibX repository..."; \
		git clone https://github.com/42Paris/minilibx-linux.git $(MLX_DIR) >/dev/null 2>&1; \
	fi
	@echo "Compiling MiniLibX..."
	@make -s -C $(MLX_DIR) >/dev/null 2>&1;
	@echo "$(BOLD)MiniLibX compiled.$(RESET)"

# Build libft library by calling 'make' in LIBFT_DIR.
# This target will be executed if libft.a is missing or
# if any of the listed .c or .h files in LIBFT_DIR are modified.
# make -s: silent mode, -C: change directory
$(LIBFT):	$(LIBFT_DIR)/libft.h \
			$(LIBFT_DIR)/ft_isalpha.c \
			$(LIBFT_DIR)/ft_isdigit.c \
			$(LIBFT_DIR)/ft_isalnum.c \
			$(LIBFT_DIR)/ft_isascii.c \
			$(LIBFT_DIR)/ft_isprint.c \
			$(LIBFT_DIR)/ft_strlen.c \
			$(LIBFT_DIR)/ft_toupper.c \
			$(LIBFT_DIR)/ft_tolower.c \
			$(LIBFT_DIR)/ft_strncmp.c \
			$(LIBFT_DIR)/ft_strlcpy.c \
			$(LIBFT_DIR)/ft_strlcat.c \
			$(LIBFT_DIR)/ft_strchr.c \
			$(LIBFT_DIR)/ft_strrchr.c \
			$(LIBFT_DIR)/ft_strnstr.c \
			$(LIBFT_DIR)/ft_memset.c \
			$(LIBFT_DIR)/ft_memchr.c \
			$(LIBFT_DIR)/ft_memcpy.c \
			$(LIBFT_DIR)/ft_memcmp.c \
			$(LIBFT_DIR)/ft_memmove.c \
			$(LIBFT_DIR)/ft_bzero.c \
			$(LIBFT_DIR)/ft_atoi.c \
			$(LIBFT_DIR)/ft_strdup.c \
			$(LIBFT_DIR)/ft_calloc.c \
			$(LIBFT_DIR)/ft_substr.c \
			$(LIBFT_DIR)/ft_strjoin.c \
			$(LIBFT_DIR)/ft_strtrim.c \
			$(LIBFT_DIR)/ft_split.c \
			$(LIBFT_DIR)/ft_itoa.c \
			$(LIBFT_DIR)/ft_strmapi.c \
			$(LIBFT_DIR)/ft_striteri.c \
			$(LIBFT_DIR)/ft_putchar_fd.c \
			$(LIBFT_DIR)/ft_putstr_fd.c \
			$(LIBFT_DIR)/ft_putendl_fd.c \
			$(LIBFT_DIR)/ft_putnbr_fd.c \
			$(LIBFT_DIR)/ft_lstnew.c \
			$(LIBFT_DIR)/ft_lstadd_front.c \
			$(LIBFT_DIR)/ft_lstsize.c \
			$(LIBFT_DIR)/ft_lstlast.c \
			$(LIBFT_DIR)/ft_lstadd_back.c \
			$(LIBFT_DIR)/ft_lstdelone.c \
			$(LIBFT_DIR)/ft_lstclear.c \
			$(LIBFT_DIR)/ft_lstiter.c \
			$(LIBFT_DIR)/ft_lstmap.c \
			$(LIBFT_DIR)/ft_strcmp.c \
			$(LIBFT_DIR)/ft_isbinary.c \
			$(LIBFT_DIR)/get_next_line_bonus.c \
			$(LIBFT_DIR)/ft_printf_utils.c \
			$(LIBFT_DIR)/ft_printf.c \
			$(LIBFT_DIR)/ft_atoi_base.c
	@make -s -C $(LIBFT_DIR)
	@echo ""

# Compilation of program; depends on $(OBJS) and library files
$(NAME):	$(OBJS) $(LIBFT) $(LIBMLX)
	@$(CC) $(CFLAGS) $(OBJS) $(LIB_FLAGS) -o $(NAME)
	@echo "$(BOLD)$(YELLOW)\n$(NAME) successfully compiled.$(RESET)"

# Print logo and usage message
	@printf "$(BOLD)$(GREEN)"

	@echo "       _      _ ___ _____ "
	@echo " _ __ (_)_ _ (_) _ \_   _|"
	@echo "| '  \| | ' \| |   / | | "
	@echo "|_|_|_|_|_||_|_|_|_\ |_|  "

	@echo "$(RESET)"

	@echo "by Natalie Holbrook & Alex Schenk @42Berlin, December 2024"
	@echo "\n$(BOLD)$(YELLOW)Usage: './$(NAME) <scene.rt>'$(RESET)"

##########
# RULES #
#########

# Target to remove all generated files BUT the program executable and compiled libraries.
clean:
	@rm -rf $(OBJS_DIR)
	@make -s -C $(LIBFT_DIR) clean  >/dev/null 2>&1
	@rm -rf $(MLX_DIR)/obj
	@echo "$(BOLD)$(RED)Object files removed.$(RESET)"

# Target to remove all generated files and the program executable (NOT the compiled libraries).
fclean:	clean
	@rm -f $(NAME)
	@echo "$(BOLD)$(RED)$(NAME) removed.$(RESET)"

# Target to remove all generated files and the program executable (NOT the compiled libraries).
fclean_all:	fclean
	@make -s -C $(LIBFT_DIR) fclean  >/dev/null 2>&1
	@rm -rf $(MLX_DIR)
	@echo "$(BOLD)$(RED)Library files removed.$(RESET)"

# Target to remove all object files, the program executable,
# and then rebuild the program.
re:	fclean
	@echo ""
	@$(MAKE) -s all

# Target to remove all object files, the program executable, and the compiled libraries,
# and then rebuild the program.
re_all:	fclean_all
	@echo ""
	@$(MAKE) -s all

.PHONY:	all clean fclean re
