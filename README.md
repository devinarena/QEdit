# QEdit
### A terminal editor designed for simplicity.
### Note: QEdit is very early in development and still lacks core functionality at this time.

## About
QEdit is a simple, lightweight terminal editor written in C designed for making simple text edits. It currently does not have much support beyond basic text editing, but more features are planned.

## Usage
Currently QEdit is only working on Windows. To use QEdit, clone the repository (or download the source) and from the root directory run
```
make
```

Once the program has finished building, you can edit a file with
```
qedit <file>
```

## Basic File Editing
Here are some basic keyboard strokes to remember for QEdit:
```
---------------------------------------------------------------------------
| Key            | Function                                               |
---------------------------------------------------------------------------
| Left Arrow     | Move document cursor left                              |
| Right Arrow    | Move document cursor right                             |
| Up Arrow       | Move document cursor up                                |
| Down Arrow     | Move document cursor down                              |
---------------------------------------------------------------------------
| Backspace      | Delete previous character                              |
| Delete         | Delete current character                               |
| Enter          | Place a \n character                                   |
| Home           | Rerender the window                                    |
---------------------------------------------------------------------------
| CTRL+S         | Save the current file                                  |
| CTRL+R         | Reload the current file (loses changes)                |
| CTRL+Q         | Quit the current editor with saving                    |
| CTRL+W         | Quit the current editor without saving                 |
---------------------------------------------------------------------------
| Any other key  | Text input (e.g. a-z, 0-9)                             |
---------------------------------------------------------------------------
```
