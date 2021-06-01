# Regex Benchmark Container
This repo contains a docker image that will install and build RE2 and PCRE2 
libraries. The goal of these tests is to compare each libraries substring 
extraction speed.

## Installation
- Docker must be installed.
- YAML file containing unit tests for benchmarking.
```YAML
---
date: 2021-05-27
version: 0
tests:
-
  name: test_example_1
  type: CEF
  result: true
  regex: <your regex pattern here>
  sample: <sample message for extraction>
-
  <next tests....>
```

## Usage
```zsh
cd <into this cloned repo>
make container
make compile
make run
```
Additionally, the PCRE2 documentation provides a very helpful demo that is also
 included in this repo.
```
# You can also compile and test the PCRE2 demo application as follows:
cd <into this cloned repo>
make container
make compile-pcre2-demo
make run-pcre2-demo PATTERN="\"h.*o world (?<name>\w*)\"" SUBJECT="\"hello world michael\""
```