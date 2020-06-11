from collections import defaultdict
import time

class Page():
  DAMPING_FACTOR = 0.15

  def __init__(self, name):
    self._weight = 1.0
    self._name = name
    self._linked_pages = []
    self._next_weight = 0.0

  def add_link(self, node):
    self._linked_pages.append(node)

  def name(self):
    return self._name
  def linked_pages(self):
    return self._linked_pages
  def weight(self):
    return self._weight

  # Returns the score that will be distributed to all other nodes.
  def distribute(self):
    if len(self._linked_pages) == 0:
      return self._weight
    weight_to_distribute = self._weight * (1 - Page.DAMPING_FACTOR) / len(self._linked_pages)
    for page in self._linked_pages:
      page._next_weight += weight_to_distribute
    return self._weight * Page.DAMPING_FACTOR

  # Update the weight for the next step.
  def update_weight(self, damping_score):
    self._weight = self._next_weight + damping_score
    self._next_weight = 0

def read_pages(file_path):
  pages = []
  title_to_page = {}
  with open(file_path) as f:
    for line in f:
      id, title = line.strip().split('\t')
      page = Page(title)
      if len(pages) != int(id):
        raise "dame"
      pages.append(page)
      title_to_page[title] = page
  return pages, title_to_page

def read_links(file_path, pages):
  with open(file_path) as f:
    for line in f:
      from_id, to_id = line.strip().split('\t')
      pages[int(from_id)].add_link(pages[int(to_id)])

def step_pagerank(pages):
  total_damping_weight = 0
  for page in pages:
    total_damping_weight += page.distribute()
  for page in pages:
    page.update_weight(total_damping_weight / len(pages))

def main():
  before = time.time()
  pages, title_to_page = read_pages('pages.txt')
  after = time.time()
  print('Read pages: %.3f [s]'%((after - before)))

  before = time.time()
  read_links('links.txt', pages)
  after = time.time()
  print('Read links: %.3f [s]'%((after - before)))

  for step in range(10):
    before = time.time()
    step_pagerank(pages)
    after = time.time()
    print('Pagerank (step %d): %.3f [s]'%(step, (after - before)))

  # title = input('> ')
  # print('weight: %d' % len(title_to_page[title].weight()))
  sorted_pages = sorted(pages, key=lambda page: page.weight(), reverse=True)
  for i in range(10):
    page = sorted_pages[i]
    print('name: %s\t\tweight: %f' % (page.name(), page.weight()))


if __name__ == '__main__':
  main()