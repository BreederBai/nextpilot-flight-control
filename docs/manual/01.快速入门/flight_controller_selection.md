# 硬件支持

Flight controllers are the "brains" of an unmanned vehicle.
Nextpilot can run on [many flight controller boards](../flight_controller/README.md).

You should select a board that suits the physical constraints of your vehicle, the activities you wish to perform, and of course cost.

<img src="/assets/flight_controller/pixhawk6x/pixhawk6x_hero_upright.png" width="130px" title="Holybro Pixhawk6X"> <img src="/assets/flight_controller/cuav_pixhawk_v6x/pixhawk_v6x.jpg" width="230px" title="CUAV Pixhawk 6X" > <img src="/assets/flight_controller/cube/orange/cube_orange_hero.jpg" width="300px" title="CubePilot Cube Orange" />

## Pixhawk Series

[Pixhawk Series](../flight_controller/pixhawk_series.md) open-hardware flight controllers run Nextpilot on RT-Thread OS.
With many form factors, there are versions targeted towards many use cases and market segments.

[Pixhawk Standard Autopilots](../flight_controller/autopilot_pixhawk_standard.md) are used as the Nextpilot reference platform.
They are supported and tested by the Nextpilot development team, and are highly recommended.

## Manufacturer-supported Controllers

Other flight controllers are [manufacturer-supported](../flight_controller/autopilot_manufacturer_supported.md).
This includes FCs that are heavily based on the Pixhawk standard (but which are not fully compliant), and many others.

Note that manufacturer-supported controllers can be just as "good" (or better) than those which are Pixhawk-standard.

## Autopilots for Computationally Intensive Tasks

Dedicated flight controllers like Pixhawk are not usually well-suited for general purpose computing or running computationally intensive tasks.
For more computing power, the most common approach is to run those applications on a separate onboard [Companion Computer](../companion_computer/README.md).
Some companion computers can also run Nextpilot on a separate DSP, as part of the same autopilot board.

Similarly, Nextpilot can also run natively Raspberry Pi (this approach is not generally considered as "robust" as having a separate companion or using a dedicated DSP):

- [Raspberry Pi 2/3 Navio2](../flight_controller/raspberry_pi_navio2.md)
- [Raspberry Pi 2/3/4 PilotPi Shield](../flight_controller/raspberry_pi_pilotpi.md)

## Commercial UAVs that can run Nextpilot

Nextpilot is available on many popular commercial drone products, including some that ship with Nextpilot and others that can be updated with Nextpilot (allowing you to add mission planning and other Nextpilot Flight modes to your vehicle).

For more information see [Complete Vehicles](../complete_vehicles/README.md).
