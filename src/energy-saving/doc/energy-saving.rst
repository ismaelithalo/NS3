Example Module Documentation
----------------------------

.. include:: replace.txt
.. highlight:: cpp

.. heading hierarchy:
   ------------- Chapter
   ************* Section (#.#)
   ============= Subsection (#.#.#)
   ############# Paragraph (no number)

This is a suggested outline for adding new module documentation to |ns3|.
See ``src/click/doc/click.rst`` for an example.

The introductory paragraph is for describing what this code is trying to
model.

For consistency (italicized formatting), please use |ns3| to refer to
ns-3 in the documentation (and likewise, |ns2| for ns-2).  These macros
are defined in the file ``replace.txt``.

Model Description
*****************

The source code for the new module lives in the directory ``src/energy-saving``.

Add here a basic description of what is being modeled.

Design
======

Briefly describe the software design of the model and how it fits into
the existing ns-3 architecture.

The ENB energy consume model is based on the mmwave-energy module. On the mmwave module (base for mmwave-energy), the phy spectrum of the mmwave enb have 4 states: IDLE = 0, TX = 1, RX_DATA = 2, RX_CTRL = 3. Therefore, on the mmwave-energy-enb, almost each state is associated with a power consumption value. More specifcaly, the power consumption is associated with the type of state. For example, IDLE, TX and RX, have a power consumption value, but RX_CTRL is considered as RX.

On the current energy model, that is based on standard Lte module, the lte phy spectrum for enb specifies 7 states: IDLE = 0, TX_DL_CTRL = 1, TX_DATA = 2, TX_UL_SRS = 3, RX_DL_CTRL = 4, RX_DATA = 5, RX_UL_SRS = 6. Following the same idea of the mmwave-energy, we can consider that RX_DL_CTRL and RX_UL_SRS are considered as RX and TX_DL_CTRL, TX_DATA and TX_UL_SRS are considered as TX.

For practical and time reasons, the value of each state is the same as the mmwave-energy module. However, the values can be changed in the future. Therefore, the power consumption values (actually current) are:

IDLE: 86.3 A
TX: 742.2 A
RX: 138.9 A

Although the module being based on the standard lte module, a change needed to be done on the lte phy spectrum file, because by default, the lte phy spectrum state is not provided as a traceble value.

Scope and Limitations
=====================

What can the model do?  What can it not do?  Please use this section to
describe the scope and limitations of the model.

References
==========

Add academic citations here, such as if you published a paper on this
model, or if readers should read a particular specification or other work.

Usage
*****

This section is principally concerned with the usage of your model, using
the public API.  Focus first on most common usage patterns, then go
into more advanced topics.

Building New Module
===================

Include this subsection only if there are special build instructions or
platform limitations.

Helpers
=======

What helper API will users typically use?  Describe it here.

Attributes
==========

What classes hold attributes, and what are the key ones worth mentioning?

Output
======

What kind of data does the model generate?  What are the key trace
sources?   What kind of logging output can be enabled?

Advanced Usage
==============

Go into further details (such as using the API outside of the helpers)
in additional sections, as needed.

Examples
========

What examples using this new code are available?  Describe them here.

Troubleshooting
===============

Add any tips for avoiding pitfalls, etc.

Validation
**********

Describe how the model has been tested/validated.  What tests run in the
test suite?  How much API and code is covered by the tests?  Again,
references to outside published work may help here.
