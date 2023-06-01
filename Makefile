NAME = webserv
CXXFLAGS = -Wall -Wextra -Werror -std=c++11 -pedantic
INCLUDE = -I include

SRC_DIR = src
BUILD_DIR = obj

SRC = main.cpp

OBJ := $(addprefix $(BUILD_DIR)/, $(SRC:.cpp=.o))
SRC := $(addprefix $(SRC_DIR)/, $(SRC))

# COLORS
RED		= \x1b[31m
GREEN	= \x1b[32m
YELLOW	= \x1b[33m
BLUE	= \x1b[34m
PINK	= \x1b[35m
CYAN	= \x1b[36m
RESET	= \x1b[0m
BRIGHT	= \x1b[1m

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $^ -o $(NAME)
	@echo "$(BRIGHT)$(CYAN)W$(PINK)E$(BLUE)B$(YELLOW)S$(GREEN)E$(RED)R$(CYAN)V$(RESET)$(GREEN) COMPILED SUCCESFULLY$(RESET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(INCLUDE) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
	@echo "$(BRIGHT)$(CYAN)W$(PINK)E$(BLUE)B$(YELLOW)S$(GREEN)E$(RED)R$(CYAN)V$(RESET)$(GREEN) CLEANED SUCCESFULLY$(RESET)"

fclean: clean
	rm -rf $(NAME)
	@echo "$(BRIGHT)$(CYAN)W$(PINK)E$(BLUE)B$(YELLOW)S$(GREEN)E$(RED)R$(CYAN)V$(RESET)$(GREEN) FANCY CLEANED SUCCESFULLY$(RESET)"

re: fclean all

debug: CXXFLAGS = -fsanitize=address 
debug: re

.PHONY: all, clean, fclean, re, debug
