.PHONY = clean

CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -pedantic -O2 -MD

CPP := g++
CPPFLAGS := -std=c++17 -Wall -Wextra -pedantic -O2 -fno-exceptions -fno-rtti -MD

LDFLAGS := \

APP_NAME := h264iframedecoder

C_OBJS := \

CPP_OBJS := \
    main.o \
    mpeg2ts_parser.o \
    h264_parser.o \
    h264_cabac_decoder.o \
    h264_decoder.o \
    picture.o \
    picture_cavlc.o \
    picture_cabac.o \

OBJS := $(C_OBJS) $(CPP_OBJS)

all: $(APP_NAME)

$(APP_NAME): $(OBJS)
	$(CPP) $^ $(LDFLAGS) -o $@

$(C_OBJS): %.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(CPP_OBJS): %.o: %.cpp Makefile
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	@rm -f $(APP_NAME) *.o *.d > /dev/null 2>&1

-include $(OBJS:.o=.d)
