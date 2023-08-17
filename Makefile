NAME = webserv
CXXFLAGS = -Wall -Wextra -Werror -std=c++11 -pedantic
# CXXFLAGS =  -std=c++11 
INCLUDE = -I ./include 

SRC_DIR = src
BUILD_DIR = obj

SRC = 	main.cpp \
		Webserver.cpp \
		requestProcessing/Request.cpp \
		requestProcessing/RequestGetters.cpp \
		requestProcessing/RequestSetters.cpp \
		requestProcessing/Response.cpp \
		requestProcessing/ResponseOCF.cpp \
		requestProcessing/ResponseGetters.cpp \
		requestProcessing/PostCGI.cpp \
		requestProcessing/delete.cpp \
		requestProcessing/utils.cpp \
		Socket.cpp \
		Connection.cpp \
		classes/Server.cpp \
		classes/Location.cpp \
		parser/parse.cpp \
		parser/utils/utils.cpp \
		parser/utils/trimUtils.cpp \
		parser/utils/checkExit.cpp \
		parser/utils/errorExit.cpp \
		parser/utils/print.cpp \
		parser/blocks/http.cpp \
		parser/blocks/server.cpp \
		parser/blocks/location.cpp \
		parser/blocks/directive.cpp \
		parser/blocks/directives/listen.cpp \
		parser/blocks/directives/serverName.cpp \
		parser/blocks/directives/root.cpp \
		parser/blocks/directives/index.cpp \
		parser/blocks/directives/autoindex.cpp \
		parser/blocks/directives/maxBodySize.cpp \
		parser/blocks/directives/errorPage.cpp \
		parser/blocks/directives/allow.cpp \
		parser/blocks/directives/deny.cpp \
		parser/blocks/directives/return.cpp \
		parser/blocks/directives/uploadDir.cpp \
		parser/blocks/directives/cgi.cpp

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
	@echo "$(CYAN)██╗    ██╗$(PINK)███████╗$(BLUE)██████╗ $(YELLOW)███████╗$(GREEN)███████╗$(RED)██████╗ $(CYAN)██╗   ██╗$(RESET)"
	@echo "$(CYAN)██║    ██║$(PINK)██╔════╝$(BLUE)██╔══██╗$(YELLOW)██╔════╝$(GREEN)██╔════╝$(RED)██╔══██╗$(CYAN)██║   ██║$(RESET)"
	@echo "$(CYAN)██║ █╗ ██║$(PINK)█████╗  $(BLUE)██████╔╝$(YELLOW)███████╗$(GREEN)█████╗  $(RED)██████╔╝$(CYAN)██║   ██║$(RESET)"
	@echo "$(CYAN)██║███╗██║$(PINK)██╔══╝  $(BLUE)██╔══██╗$(YELLOW)╚════██║$(GREEN)██╔══╝  $(RED)██╔══██╗$(CYAN)╚██╗ ██╔╝$(RESET)"
	@echo "$(CYAN)╚███╔███╔╝$(PINK)███████╗$(BLUE)██████╔╝$(YELLOW)███████║$(GREEN)███████╗$(RED)██║  ██║$(CYAN) ╚████╔╝ $(RESET)"
	@echo "$(CYAN) ╚══╝╚══╝ $(PINK)╚══════╝$(BLUE)╚═════╝ $(YELLOW)╚══════╝$(GREEN)╚══════╝$(RED)╚═╝  ╚═╝$(CYAN)  ╚═══╝  $(RESET)$(BRIGHT)$(GREEN)COMPILED SUCCESFULLY$(RESET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(INCLUDE) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -rf $(BUILD_DIR)
	@echo "$(BRIGHT)$(CYAN)W$(PINK)E$(BLUE)B$(YELLOW)S$(GREEN)E$(RED)R$(CYAN)V$(RESET)$(GREEN) CLEANED SUCCESFULLY$(RESET)"

fclean: clean
	rm -rf $(NAME)
	@echo "$(BRIGHT)$(CYAN)W$(PINK)E$(BLUE)B$(YELLOW)S$(GREEN)E$(RED)R$(CYAN)V$(RESET)$(GREEN) FULLY CLEANED SUCCESFULLY$(RESET)"

re: fclean all

test: all 
	@bash ./error_tests/test.sh

debug: CXXFLAGS += -g -fsanitize=address 
debug: re

.PHONY: all, clean, fclean, re, debug, test
