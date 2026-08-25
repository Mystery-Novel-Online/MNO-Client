# Style Guide

This document will outline the preferred coding style and conventions to be followed in this repository.

---

## General

### Braces
- Opening braces go at the end of the last line used in an if statement or function decleration with a space seperating it, and not at the start of the next line. 
- Closing braces go on the last line of a function or if statement by itself, or on the same line as the open curly brace. 

---

## Naming Conventions

### Function Names
- Use **camelCase** for function names to keep things consistent with Qt function calls. 

### Variables
- Use **camelCase** for variable names.
- Prefix with **a_** for function arguments.
- Prefix with **m_** for non-static member variables.
- Prefix with **g_** for static member variables. 
- Prefix with **u_** for Qt widgets.

---

## Functions
- Basic accessor functions can be defined as inline in the header. 

---

## Qt Signals
- When using connect with a lambda function, it must be split across 3 lines. Anything above this should be split into a slot. 

--

## Includes
- When including from an external library, use **< >**.
- When including from within the project itself, use **" "**.