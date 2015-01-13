bin_PROGRAMS = multiman multiman_server

BUILT_SOURCES = main.x

# libconfigfiles_la_SOURCES = grammar.yy  lexer.ll  parser.h  token.h lexer.h config_creator.h configfile.h configfile.cpp
multiman_SOURCES = 	main.cpp \
			cmdline.cpp cmdline.h \
			wall-timer.h \
			rendering.cpp rendering.h \
			drawelement.cpp drawelement.h \
			objloader.cpp objloader.h \
			messages.cpp messages.h \
			clientside-networking.cpp clientside-networking.h \
			heightmap.cpp heightmap.h \
            simple_heightmap.cpp simple_heightmap.h \
            mouseactions.cpp mouseactions.h \
            menu.cpp menu.h \
            gameobject.cpp gameobject.h \
            game.cpp game.h

multiman_server_SOURCES = 	server-logic.cpp server-logic.h \
				server-main.cpp \
				messages.cpp messages.h 

snarf_opts = $(AM_CPPFLAGS) $(multiman_CPPFLAGS) $(CPPFLAGS)
SUFFIXES: .x
.cpp.x:
	guile-snarf -o $@ $< $(snarf_opts)

# for bison/flex
# AM_CPPFLAGS = -Isrc/
# AM_YFLAGS = -d -p__yacc_prefix__
# AM_LFLAGS = -P__lex_prefix__ -o lex.yy.c

#library_includedir = $(includedir)/$(PACKAGE)-$(VERSION)
#library_include_HEADERS = multiman.h

# setting c++ flags:
# AM_CXXFLAGS = -Wall -ansi -pedantic -ggdb -pg

# setting c preprocessor flags for main project
multiman_CPPFLAGS = -D PACKAGE=\"$(PACKAGE)\" -D VERSION=\"$(VERSION)\" $(CGL_CFLAGS) $(CCGL_CFLAGS) $(GLEW_FLAGS) -std=c++11
multiman_LDADD = $(CGL_LIBS) $(CCGL_LIBS) $(GLEW_LIBS) $(BOOST_ASIO_LIB)

multiman_server_CPPFLAGS = -D PACKAGE=\"$(PACKAGE)\" -D VERSION=\"$(VERSION)\" $(BOOST_CPPFLAGS)
multiman_server_LDADD = $(BOOST_ASIO_LIB)

main.cpp: main.x
