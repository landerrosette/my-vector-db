# Simple Vector Database

A basic vector database supporting vector indexing (using Faiss and HNSWLib), scalar data storage (using RocksDB), and
filtering. It exposes its functionality via an HTTP API.

## Development Environment

The recommended way to set up the development environment and handle dependencies is by using the provided Dev Container
configuration (`.devcontainer` folder).

**Prerequisites:**

1. **Docker:** Ensure Docker is installed and running on your system.
2. **VS Code or JetBrains Gateway/CLion:** Install one of these editors with the appropriate "Dev Containers" or "Remote
   Development" extension installed.

**Steps:**

1. **Clone the repository:** Ensure you clone the repository including its submodules.

   ```shell
   git clone --recursive https://github.com/landerrosette/my-vector-db.git
   ```

2. **Open the project:** Open the cloned project folder in VS Code or JetBrains IDE.
3. **Reopen in Container:** Your editor should detect the Dev Container configuration and prompt you to "Reopen in
   Container" (VS Code) or similar. Accept the prompt.
4. **Wait for Container setup:** Docker will build the image (based on the `Dockerfile`) and start a container with all
   necessary
   dependencies pre-installed and the project code mounted. This might take a few minutes the first time.

You are now working inside the container environment, ready to build and run the project.

## Building and Running

Once you are inside the Dev Container environment, you can build and run the project from the integrated terminal:

1. **Build:**

   ```shell
   cmake -B build
   cmake --build build
   ```

   This will create the `vector-db` executable in the `build` directory.

2. **Run:**

   ```shell
   build/vector-db
   ```

   The server will start on `0.0.0.0:8080` inside the container. You might need to configure port forwarding in your
   editor or Docker setup to access this port from your host machine.

   **Note:** Some initial parameters are currently hardcoded in `main.cpp`.
    - The vector dimension is set to `1`.
    - The HTTP server listens on `0.0.0.0:8080`.
    - The global log level is set to `DEBUG`.

   These values can be modified by editing `main.cpp` and rebuilding the project.

## API Endpoints

The server exposes the following HTTP endpoints, expecting `Content-Type: application/json` for POST requests. Responses
are also JSON and include a `retCode` (0 for success).

### `POST /upsert`

Inserts a new data entry or updates an existing one based on the `id`. Includes vector data and optional scalar fields.

#### Request Body

```json
{
    "id": 3,
    "vectors": [0.555555],
    "indexType": "FLAT",
    "Name": "hello",
    "Ci": 1111
}
```

- `id`: (Required, integer) The unique identifier for the data entry.
- `vectors`: (Required, array of float) The vector data.
- `indexType`: (Required, string) Specifies the vector index type to use for storing this vector ("FLAT" or "HNSW").
- Any other fields (e.g., "Name", "Ci"): (Optional, various types) These fields are stored as scalar data. Integer
  fields can be used for filtering in search.

### `POST /search`

Searches for nearest neighbors based on a query vector, with optional filtering.

#### Request Body

```json
{
    "vectors": [0.8],
    "k": 5,   
    "indexType": "FLAT", 
    "filter": {
        "fieldName": "Ci",
        "value": 1111,              
        "op": "="                 
    }
}
```

- `vectors`: (Required, array of float) The query vector.
- `k`: (Required, integer) The number of nearest neighbors to return.
- `indexType`: (Required, string) Specifies which vector index to perform the search on ("FLAT" or "HNSW").
- `filter`: (Optional, object) An object specifying filtering criteria.
    - `fieldName`: (Required if `filter` is present, string) The name of the scalar field to filter on. **Currently only
      supports integer fields**.
    - `value`: (Required if `filter` is present, integer) The value used for filtering.
    - `op`: (Required if `filter` is present, string) The comparison operation. (`=` (equal to) or `!=` (not equal
      to)).

### `POST /query`

Retrieves the data associated with a given ID.

#### Request Body

```json
{
    "id": 3
}
```

- `id`: (Required, integer) The ID of the data entry to retrieve.

### `GET /admin/snapshot`

Triggers a snapshot of the current index state to disk. No request body is required for this GET endpoint.

## Persistence

The database uses a Write-Ahead Log (WAL, `index_wal.log`) and snapshots (`snapshot` directory) to persist data across
restarts. On startup, it loads the last snapshot and replays any WAL entries since that snapshot. These files will be
created within the project directory inside the container.
