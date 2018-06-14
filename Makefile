CFLAGS += -O3 -std=c++14 -Wall -Wextra

PAGERANK_SRCS := pagerank.cc
PAGERANK_FOR_WIKIPEDIA_SRCS := homework2_cpp/pagerank_for_wikipedia.cc

BINDIR = bin

.PHONY: all
all: $(BINDIR)/pagerank_for_wikipedia $(BINDIR)/pagerank

$(BINDIR)/pagerank_for_wikipedia: $(PAGERANK_FOR_WIKIPEDIA_SRCS) $(BINDIR)
	$(CXX) $(CFLAGS) -o $@ $(PAGERANK_FOR_WIKIPEDIA_SRCS)

$(BINDIR)/pagerank: $(PAGERANK_SRCS) $(BINDIR)
	$(CXX) $(CFLAGS) -o $@ $(PAGERANK_SRCS)

$(BINDIR):
	mkdir -p $(BINDIR)

.PHONY: clean
clean:
	rm -rf $(BINDIR)
