import os


def get_registry_path() -> str:
    return os.path.join(os.path.dirname(__file__), "message_registry.json")


def get_comp_id_file_path(creatorID: str) -> str:
    return os.path.join(
        os.path.dirname(__file__), creatorID + "_component_ids.json"
    )
