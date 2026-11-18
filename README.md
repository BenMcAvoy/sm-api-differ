# sm-api-differ

sm-api-differ compares two Scrap Mechanic Lua API dumps and builds a browsable diff. It matches classes, userdatas and namespaces, then records entries as added, removed, modified or unchanged.

## Generate the diff

The two input snapshots are stored in `json_ch1/` and `json_ch2/`. With [xmake](https://xmake.io) installed, run:

```sh
xmake
xmake run cpp
```

The program writes the comparison to `docs/diff.json`. Open `docs/index.html` to view the browser interface against that data.

## Status

This is a focused documentation tool for comparing the bundled API snapshots; it does not fetch game data by itself.
