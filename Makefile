DEBUG = -g
CFLAGS = -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -c $(DEBUG)
LFLAGS = -Wall -lm

SRCDIR := src
BUILDDIR := build
OBJDIR := $(BUILDDIR)/obj

NAME := build/tsh

override SRCS := $(shell find src/*.cpp -type f)

override OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.cpp=.o)))

all: $(NAME)

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CFLAGS) $< -o $@

$(NAME): $(OBJS)
	$(CXX) -o $(NAME) $^ $(LFLAGS)

clean:
	rm -rf $(OBJDIR)
	rm -rf $(NAME)
