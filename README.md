# My Vector DB

A simple vector storage and retrieval service.

## Setup

The recommended way to set up the development environment and handle dependencies is by using the provided Dev Container
configuration (`.devcontainer` folder).

### Prerequisites

- Docker
- An editor/IDE with Dev Container support (e.g., VS Code or JetBrains Gateway/CLion)

### Steps

1. Clone the repository. **Use `--recursive` to clone the repository including its submodules.**

    ```shell
    git clone --recursive https://github.com/landerrosette/my-vector-db.git
    ```

2. Open the cloned project folder in your editor or IDE.
3. Your editor should detect the Dev Container configuration and prompt you to "Reopen in Container" (VS Code) or
   similar. Accept the prompt.
4. Wait for Docker to build the image (based on the `Dockerfile`) and start a container with all necessary dependencies
   pre-installed and the project code mounted. This might take a few minutes the first time.

You are now working inside the container environment, ready to build and run the project.

## Build and Run

Once you are inside the container, you can build and run the project from the integrated terminal.

1. Configure and build the project in the `build` directory.

    ```shell
    cmake -B build
    cmake --build build
    ```

2. Run the application:

    ```shell
    build/vector-db
    ```

   The server will now start on `0.0.0.0:8080` inside the container. You might need to configure port forwarding
   to access this port from your host machine.

   **Note:** Some initial parameters are currently hardcoded in `main.cpp`.

    - The vector dimension is set to `1`.
    - The HTTP server listens on `0.0.0.0:8080`.
    - The global log level is set to `DEBUG`.

   These values can be modified by editing `main.cpp` and rebuilding the project.

## HTTP Endpoints

The server exposes the following HTTP endpoints, expecting `Content-Type: application/json` for POST requests. Responses
are also JSON and include a `retCode` (0 for success).

### `POST /upsert`

Inserts a new data entry or updates an existing one based on the `id`. Includes vector data and optional scalar fields.

Request Body:

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

Request Body:

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
    - `fieldName`: (Required if `filter` is present, string) The name of the scalar field to filter on. Only
      supports integer fields.
    - `value`: (Required if `filter` is present, integer) The value used for filtering.
    - `op`: (Required if `filter` is present, string) The comparison operation. (`=` (equal to) or `!=` (not equal
      to)).

### `POST /query`

Retrieves the data associated with a given ID.

Request Body:

```json
{
    "id": 3
}
```

- `id`: (Required, integer) The ID of the data entry to retrieve.

### `GET /admin/snapshot`

Triggers a snapshot of the current index state to disk. No request body is required for this GET endpoint.
