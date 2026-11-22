# duckdb_mooncake

[![Website: mooncake.pgsty.com](https://img.shields.io/badge/Website-mooncake.pgsty.com-slategray?style=flat&logo=cilium&logoColor=white)](https://mooncake.pgsty.com/)
[![Docs: duckdb_mooncake](https://img.shields.io/badge/Docs-duckdb__mooncake-slategray?style=flat)](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/)
[![Version: v0.0.4](https://img.shields.io/badge/Version-v0.0.4-slategray?style=flat)](https://github.com/pgsty/duckdb_mooncake/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-slategray?style=flat&logo=opensourceinitiative&logoColor=green)](https://github.com/pgsty/duckdb_mooncake/blob/main/LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/pgsty/duckdb_mooncake?style=flat&logo=github&logoColor=black&color=slategray)](https://star-history.com/#pgsty/duckdb_mooncake&Date)
[![DuckDB: v2.0.1](https://img.shields.io/badge/DuckDB-v2.0.1-%23FFF000?style=flat&logo=duckdb&logoColor=black&labelColor=FFF000)](https://duckdb.org/)

**`duckdb_mooncake`** is a DuckDB storage and catalog extension that reads real-time Apache Iceberg tables written by [`moonlink`][moonlink-link] with sub-second LSN-consistent freshness, powering vectorized analytical queries in [`pg_mooncake`](https://github.com/pgsty/pg_mooncake) and standalone DuckDB.

[Website](https://mooncake.pgsty.com/) | [Documentation](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/) | [pg_mooncake](https://github.com/pgsty/pg_mooncake) | [moonlink][moonlink-link] | [Blog](https://mooncake.pgsty.com/blog/) | [PGEXT](https://pgext.cloud/ext/pg_mooncake) | [Releases](https://github.com/pgsty/duckdb_mooncake/releases) | [Issues](https://github.com/pgsty/duckdb_mooncake/issues) | [Upstream](https://github.com/Mooncake-Labs/duckdb_mooncake)

---

## Mooncake Suite

`duckdb_mooncake` is the DuckDB storage and scan engine of the **Mooncake** family maintained by [PGSTY](https://pgsty.com):

- **[`pg_mooncake`](https://github.com/pgsty/pg_mooncake)** provides the PostgreSQL 15–19 integration through `pg_duckdb 1.2.1`.
- **[`moonlink`][moonlink-link]** maintains Iceberg mirrors from the PostgreSQL replication stream.
- **[`duckdb_mooncake`](https://github.com/pgsty/duckdb_mooncake)** reads those mirrors in DuckDB `v2.0.1`.

---

## Version v0.0.4

Each scan owns its Moonlink connection and releases its read state when the connection closes.
The version change lets pg_mooncake detect an older binary in DuckDB's v2.0.1 extension cache.

## Version v0.0.3

Version `v0.0.3` upgrades the DuckDB baseline to **DuckDB `v2.0.1`** (`8d78114e90a53823803660bf821f6073ac87da8c`) for [`pg_mooncake v0.3.1`](https://github.com/pgsty/pg_mooncake/releases/tag/v0.3.1) and `pg_duckdb 1.2.1`, and aligns the `moonlink_ffi` RPC client to [`moonlink v0.3.0`](https://github.com/pgsty/moonlink/releases/tag/v0.3.0) (`dcf999a9f342f8e74242b85cfe4d893c26f9e4be`).

## Version v0.0.2

Version `v0.0.2` is the first PGSTY release, targeting **DuckDB `v2.0.0`** (`d54cbbe3df17e8e5ce68312f483b4155836112aa`) and [`pg_mooncake v0.3.0`](https://github.com/pgsty/pg_mooncake/releases/tag/v0.3.0):

- **DuckDB `v2.0.0` C++ Storage & Catalog Migration**: Adapts `MooncakeCatalog`, `MooncakeTransactionManager`, `MultiFileReader`, `TableFilterSet` iteration, and `DUCKDB_CPP_EXTENSION_ENTRY` to DuckDB `2.0.0`.
- **PostgreSQL LSN Snapshot Barrier**: Resolves `mooncake_get_postgres_lsn()` dynamically via `dlsym(RTLD_DEFAULT, ...)` when loaded inside PostgreSQL (`pg_duckdb`) so `moonlink_create_scan_snapshot` waits for the current PostgreSQL WAL flush LSN before scanning.
- **Filter Pushdown Column-Index Remapping**: Remaps `IS NULL`, `IS NOT NULL`, and dynamic `TableFilterSet` keys from projection column order to physical Parquet file column indices before initializing `ParquetReader` scans.
- **Pinned & Audited FFI Client**: Pins `moonlink_ffi` (`moonlink_rpc`) to [`moonlink v0.0.1`](https://github.com/pgsty/moonlink/releases/tag/v0.0.1) (`b298ef7444890e3c8af5544e350232518bcfd8e5`) and upgrades fallback `CRoaring` to `v5.5.4`.

See [Adapting pg_mooncake to DuckDB 2.0 & PostgreSQL 18](https://mooncake.pgsty.com/blog/post/duckdb-2-adaptation/) and the [duckdb_mooncake Architecture Guide](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/) for details.

## Installation

With `pg_mooncake v0.3.2`, the build stages scanner `v0.0.4` for DuckDB `v2.0.1` into `$(pg_config --sharedir)/extension/pg_mooncake`. Set `mooncake.extension_repository` to this directory.

In a standalone DuckDB `v2.0.1` session configured for unsigned local extensions, load the matching build:
```sql
LOAD '/absolute/path/to/mooncake.duckdb_extension';
```

## Usage

Mooncake databases can be attached using the `ATTACH` command, after which tables can be queried using standard SQL.

The example below attaches to the moonlink database `'postgres'`, from a moonlink instance listening at `'/var/lib/postgresql/data/pg_mooncake/moonlink.sock'`. This moonlink instance comes prepopulated with a table named `public.c`:
```sql
D ATTACH DATABASE 'mooncake' (TYPE mooncake, URI '/var/lib/postgresql/data/pg_mooncake/moonlink.sock', DATABASE 'postgres');
D SELECT * FROM mooncake.public.c;
┌───────┬─────────┐
│  id   │   val   │
│ int32 │ varchar │
├───────┼─────────┤
│     1 │ Hello   │
│     2 │ World   │
└───────┴─────────┘
```

## Building

To build, type:
```bash
git submodule update --init --recursive
GEN=ninja make
```

To run, run the bundled `duckdb` shell:
```bash
./build/release/duckdb
```

---

## Documentation

Full documentation is available at **[mooncake.pgsty.com/docs/intro/duckdb-mooncake](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/)**:

- [duckdb_mooncake Scanner Architecture](https://mooncake.pgsty.com/docs/intro/duckdb-mooncake/)
- [Mooncake End-to-End Architecture](https://mooncake.pgsty.com/docs/intro/architecture/)
- [Adapting pg_mooncake to DuckDB 2.0 & PostgreSQL 18](https://mooncake.pgsty.com/blog/post/duckdb-2-adaptation/)
- [Unlocking DuckDB's Parallel Parquet Reader for Custom Table Functions](https://mooncake.pgsty.com/blog/post/duckdb-parquet/)

---

## License

This software is licensed under the **MIT License ([MIT](LICENSE))**.

- Copyright (c) 2018–2025 Stichting DuckDB Foundation
- Copyright (c) 2025 Mooncake Labs, Inc.
- Copyright (c) 2026 PGSTY PTE. LTD.
- Maintained by [Ruohang Feng](mailto:rh@vonng.com) @ [PGSTY](https://pgsty.com)

[moonlink-link]: https://github.com/pgsty/moonlink
