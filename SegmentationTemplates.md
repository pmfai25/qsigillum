# Preconditions #

Templates should contain values of minimal input image size that will also act as norm values for bigger size images.

# Template structure #

A template consists of a number of **containers**. Coordinates and size of a container can be set by template author.

Containers, at least at this moment, are supposed to have horizontal layout. A container consists of a number of **fields**.

A container can be marked as _repeatable_: it means that this container is factically a row of a table of unknown row count. Row interval value can be set.

# Segmentation process #

After the input image has been **preprocessed**, containers' list is iterated through. Area around container's marked coordinates is analyzed, and in case it contains some information, this piece of image is sent to the **classificator**; a control widget is created and its value is set to classificator's output value. If a container is marked as repeatable and at least one of its fields receives some non-null value from the classificator, then the area below the current rectangle  is checked using this container's structure.