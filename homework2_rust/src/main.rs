use std::error::Error;
use std::fs::File;
use std::io::prelude::*;
use std::io::BufReader;
use std::time::Instant;

#[derive(Debug)]
struct Page {
  name : String,
  edges : Vec<i32>,
}

impl Page {
  fn new(name : String) -> Page {
    Page{name: name, edges: Vec::new()}
  }

  fn add_edge(&mut self, edge : i32) {
    self.edges.push(edge);
  }

  fn edges<'a>(&'a self) -> &'a Vec<i32> {
    &self.edges
  }
}

fn read_pages(filepath : &str) -> Option<Vec<String>> {
  let mut pages : Vec<String>  = Vec::new();
  let mut index = 0;
  let file = match File::open(filepath) {
    Err(why) => panic!("couldn't open {}: {}", filepath,
                       Error::description(&why)),
    Ok(file) => file,
  };
  let reader = BufReader::new(file);
  for line in reader.lines() {
    let line = line.expect("line doesn't exist");
    let mut values = line.split_whitespace();
    let id = values.next().expect("id doesn't exist");
    let id : i32 = id.parse().expect("id should be a number");
    let name = values.next().expect("name doesn't exist");
    let name : String = name.parse().expect("name should be a String");

    if id != index {
      return None;
    }
    pages.push(name);
    index += 1;
  }
  Some(pages)
}

fn read_links(filepath : &str, names : Vec<String>) -> Vec<Page> {
  let mut pages : Vec<Page>  = Vec::new();
  pages.push(Page::new(names[0].clone()));
  let mut index = 0;
  let file = match File::open(filepath) {
    Err(why) => panic!("couldn't open {}: {}", "links.txt",
                       Error::description(&why)),
    Ok(file) => file,
  };
  let reader = BufReader::new(file);
  for line in reader.lines() {
    let line = line.expect("line doesn't exist");
    let mut values = line.split_whitespace();
    let from = values.next().expect("0 doesn't exist");
    let from : i32 = from.parse().expect("from should be a number");
    let to = values.next().expect("1 doesn't exist");
    let to : i32 = to.parse().expect("to should be a number");

    if from != index {
      pages.push(Page::new(names[index as usize].clone()));
      index += 1;
    }
    pages.last_mut().unwrap().add_edge(to);
  }
  pages
}

fn main() {
  println!("Hello, world!");

  println!("Read pages.txt");
  let start = Instant::now();  
  let names = read_pages("pages.txt").unwrap();
  let end = start.elapsed();
  println!("Elapsed: {}.{:03} sec", end.as_secs(), end.subsec_nanos() / 1_000_000);

  println!("Read links.txt");
  let start = Instant::now();  
  let pages = read_links("links.txt", names);
  let end = start.elapsed();

  let mut n_edges = 0;
  for page in &pages {
    n_edges += page.edges().len();
  }
  
  println!("{} pages, {} edges", pages.len(), n_edges);
  println!("Elapsed: {}.{:03} sec", end.as_secs(), end.subsec_nanos() / 1_000_000);

  println!("{:?}", pages[0]);
}
