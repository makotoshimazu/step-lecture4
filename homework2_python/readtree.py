#!/usr/bin/env python
# -*- coding: utf-8 -*-
# vim: set fileencoding=utf-8 :

import os, time

def ps():
  os.system("ps -p %d u" % os.getpid())

def read_file_dict(filename):
  graph = {}
  f = open(filename, 'r')
  for line in f:
    line = line.rstrip().split('\t')
    src = int(line[0])
    dst = int(line[1])
    if not graph.has_key(src):
      graph[src] = []
    graph[src].append(dst)
  return graph

def read_file_array(filename):
  graph = []
  vertex = []
  index = 0
  f = open(filename, 'r')
  for line in f:
    line = line.rstrip().split('\t')
    src = int(line[0])
    dst = int(line[1])
    if src != index:
      graph.append(vertex)
      vertex = []
      index += 1
    vertex.append(dst)
  graph.append(vertex)
  return graph


if __name__ == '__main__':
  filename = 'links.txt'

#   print("== dict ==")
#   start = time.time()
#   graph = read_file_dict(filename)
#   elapsed = time.time() - start
#   print("elapsed: {} sec".format(elapsed))
#   ps()
#   if (len(graph) <= 10):
#     print(graph)

  print("== array ==")
  start = time.time()
  graph = read_file_array(filename)
  elapsed = time.time() - start
  print("elapsed: {} sec".format(elapsed))
  ps()
  if (len(graph) <= 10):
    print(graph)
