import os
import shutil

from setuptools import setup

# Builds the message registry and other data files into a python package

# Copy the msg registry and comp IDs files into the subdir with
# the __init__.py before building the package so they can reside in
# ../site-packages/pel_registry/ instead of site-packages/registry.
this_dir = os.path.dirname(__file__)
target_dir = os.path.join(this_dir, "pel_registry")
shutil.copy(
    os.path.join(this_dir, "registry/message_registry.json"), target_dir
)
shutil.copy(
    os.path.join(this_dir, "registry/O_component_ids.json"), target_dir
)
shutil.copy(
    os.path.join(this_dir, "registry/B_component_ids.json"), target_dir
)

setup(
    name="pel_message_registry",
    version=os.getenv("PELTOOL_VERSION", "1.0"),
    classifiers=["License :: OSI Approved :: Apache Software License"],
    packages=["pel_registry"],
    package_data={
        "": [
            "message_registry.json",
            "O_component_ids.json",
            "B_component_ids.json",
        ]
    },
)
