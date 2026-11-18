# Damiao Motor Hardware Integration

## Short answer

The rover computer cannot connect a Damiao motor directly to USB. A
Damiao motor exposes CAN, so the minimum hardware path is:

```text
Rover computer USB -> USB-to-CAN adapter -> CAN_H/CAN_L bus -> arm motors
                                      -> separate motor power wiring
```

Use one 1 Mb/s classic-CAN bus initially. Assign every motor a unique command
ID (`ESC_ID`) and feedback ID (`MST_ID`), and fit 120 ohm termination at both
physical ends of the bus. The rover computer's USB port provides the data
connection only; motor power still comes from the rover's motor power system.

The recommended software path is **Linux SocketCAN**, because it gives the
rover a normal `can0` network interface and keeps the Damiao protocol
independent of a particular USB vendor:

1. **Preferred for the first bench test:** USB-to-CAN adapter + SocketCAN +
   the OpenArm C++ CAN library.
2. **Preferred for long-term ownership:** USB-to-CAN adapter + SocketCAN + a
   small Damiao transport/codec in the rover controls repository.

Both methods need the same USB-to-CAN adapter. The Technoculture C++ SDK is a
possible third option, but only with a serial USB-CAN bridge that implements
its adapter-specific 30-byte frame format. It is not a drop-in driver for a
generic SocketCAN, SLCAN, or CANable device.

## Protocol constraints that affect the design

Damiao uses standard 11-bit classic CAN frames with eight data bytes. The
documented control modes are:

| Mode | Command arbitration ID | Payload |
| --- | --- | --- |
| MIT | motor command ID (`ESC_ID`) | packed position, velocity, stiffness, damping, and torque |
| Position/velocity | `0x100 + ESC_ID` | little-endian float position and velocity limit |
| Velocity | `0x200 + ESC_ID` | little-endian float velocity |
| Force/position | `0x300 + ESC_ID` | position, velocity limit, and torque ratio |

System commands use the command ID, while feedback uses the configured
feedback ID (`MST_ID`). Feedback includes operating state/error, position,
velocity, torque, MOS temperature, and rotor temperature. Register requests
use ID `0x7FF`, including reading and writing `ESC_ID`, `MST_ID`, control
mode, bitrate, limits, and timeout settings.

For a four-motor arm, example assignments could be:

| Motor | `ESC_ID` | `MST_ID` |
| --- | --- | --- |
| base | `0x01` | `0x11` |
| shoulder | `0x02` | `0x12` |
| elbow | `0x03` | `0x13` |
| wrist | `0x04` | `0x14` |

These IDs are configuration choices, not the IDs currently stored in
unopened motors. Verify all IDs one motor at a time before connecting several
motors to the same bus. All motors on a shared bus must use the same CAN
bitrate, and the unused ID ranges must not collide with command-mode IDs.

## Hardware options

### Option H1: CANable 2.0 or equivalent `gs_usb` adapter

The Damiao documentation names CANable 2.0 as tested hardware, with
**candleLight firmware** on Linux. The adapter should enumerate as a native
SocketCAN interface such as `can0`.

Advantages:

- Low-cost and easy to replace.
- Native Linux `can0` interface instead of a vendor serial protocol.
- Works with `candump`, `can-utils`, Python `socketcan`, and C++.
- 1 Mb/s classic-CAN support matches the protocol.

Purchase/configuration checks:

- Confirm it really uses the `gs_usb`/candleLight firmware. The stock serial
  firmware on a CANable is an SLCAN device and requires `slcand`; some
  retailers sell clones with firmware/configuration variants.
- Prefer a unit with a stable USB serial number, galvanic isolation, and an
  accessible termination switch or external termination connections.
- Buy a small USB extension or strain-relieved enclosure for rover vibration.
- Use a real ground connection only as permitted by the adapter and motor
  wiring design. Do not rely on the USB cable for CAN ground.

Typical Linux setup:

```bash
sudo apt install can-utils
sudo ip link set can0 down 2>/dev/null || true
sudo ip link set can0 type can bitrate 1000000
sudo ip link set can0 up
ip -details -statistics link show can0
candump can0
```

The exact interface name can be stabilized with a `udev` rule matching the
adapter serial number. Do this before deployment so `can0` does not
unexpectedly refer to a different USB-CAN device after reboot.

### Option H2: i2rt USB2CAN

The Damiao setup guide also lists the i2rt USB2CAN as tested on Linux
SocketCAN and macOS `gs_usb`. It is a more purpose-built replacement for a
generic CANable while retaining the standard SocketCAN software path.

Prefer this option if it provides a more reliable connector, enclosure,
isolation, or support arrangement for the rover. Compare it against the
CANable on those mechanical/electrical features rather than software
compatibility; both expose the same `can0` abstraction.

### Bus construction requirements

- Connect the adapter and all motors on one twisted-pair `CAN_H`/`CAN_L` bus,
  with short stubs.
- Install 120 ohm between `CAN_H` and `CAN_L` at the first and last physical
  nodes. Measure approximately 60 ohms across a powered-down bus.
- Do not enable termination on every motor.
- Use keyed, lockable connectors and a separately fused motor supply.
- Add hardware emergency-stop behavior that removes motor enable/power. CAN
  software is not a safety-rated stop.
- Disable motors on shutdown and on watchdog/communication-loss conditions.
  A configured Damiao communication timeout provides an additional hardware
  safeguard.

For a first four-motor build, one USB-to-CAN channel is sufficient at 1 Mb/s:
each 8-byte command/feedback pair is small compared with available bus
bandwidth. Multiple CAN channels are an optimization for fault isolation or
different arm/control groups, not a prerequisite. If bench testing shows
latency, queueing, or wiring-fault isolation problems, use a two-channel
isolated adapter or two independent adapters.

## Software method 1: OpenArm CAN over SocketCAN

### Architecture

```text
ArmHardware -> Damiao adapter class -> openarm_can -> PF_CAN socket
                                                       |
                                                    Linux can0
                                                       |
                                                  USB-to-CAN
```

The `enactic/openarm_can` library is the fastest path to a working C++ bench
test. It:

- uses C++17, matching this repository's current build standard;
- sends raw CAN/CAN-FD frames through Linux SocketCAN;
- supports standard Damiao message encoding, motor state, parameters, control
  modes, and multiple motors;
- includes bus-health/error accounting;
- provides `openarm-can-cli` tools for discovery, monitoring, diagnostics,
  zeroing, changing IDs, and reading/writing parameters.

Its examples configure `can0`, register motor type and `ESC_ID`/`MST_ID`, enable
the motors, issue MIT commands, and receive state frames. DaMiao protocol
frames are only 8 bytes, so classic CAN is sufficient and the library can be
constructed with CAN-FD disabled.

### Fit with Controls

Add a concrete Damiao-backed motor behind the existing arm motor abstraction
rather than calling `openarm_can` directly from handlers. That keeps the arm
handler and higher-level controller independent from the CAN library.

The new adapter would own:

- interface name and one SocketCAN/openarm instance;
- joint-to-`ESC_ID`/`MST_ID` mapping;
- motor model, position offsets, direction, gear ratio, and limits;
- startup checks, enable/disable, watchdog, and shutdown;
- conversion between rover arm joint angles and motor rotor/encoder angles.

### Limitations

The OpenArm source currently has presets for a fixed list of Damiao motor
models. Before adoption, confirm that the actual selected arm motors are in
that list or add correct `PMAX`/`VMAX`/`TMAX` values after checking the
individual motor datasheet and firmware. It is unsafe to substitute
parameters from a different model.

The Python bindings in OpenArm are explicitly experimental. Controls should
use the C++ library.

## Software method 2: in-repository SocketCAN Damiao driver

### Architecture

```text
ArmHardware -> DamiaoMotorCollection -> Damiao protocol codec
                                               |
                                      PF_CAN SocketCAN transport
                                               |
                                            can0
```

This method uses the same adapter and Linux CAN configuration as method 1,
but implements only the Damiao protocol and behavior needed by the rover.
The transport is replaceable and testable:

```cpp
class CanTransport {
public:
    virtual ~CanTransport() = default;
    virtual void send(const CanFrame& frame) = 0;
    virtual bool receive(CanFrame& frame, std::chrono::microseconds timeout) = 0;
};
```

Implement the production transport with `PF_CAN`/`CAN_RAW`. Implement tests
with a mock transport or Linux `vcan`. Keep Damiao encoding and decoding in a
separate, hardware-independent class.

Minimum required protocol operations:

1. Enable and disable a motor.
2. Send the selected control mode at a fixed control-loop rate.
3. Decode feedback and retain the last state timestamp.
4. Clear faults and read temperatures/error state.
5. Read and write the small register subset required for setup.
6. Persist setup-only writes explicitly, with read-back verification.
7. Detect stale feedback, timeout, bus-off, socket errors, and queue overflow.

### Benefits

- No dependency on an arm-specific third-party API or an external package
  repository.
- Full control over real-time threading, error semantics, timing, and logging.
- Easier unit tests for protocol encoding and joint conversion.
- No vendor lock-in: any adapter that appears as a Linux SocketCAN interface
  works.
- The existing C++17 build can support Linux CAN headers directly.

### Costs

- More protocol and safety code to own and review.
- Initial bring-up takes longer than OpenArm.
- Parameter writes, mode changes, ID assignment, and watchdog handling must
  be carefully tested on real hardware; `vcan` cannot verify motor behavior.

Use method 2 if the Controls team expects to maintain several motor protocols
or needs stricter integration with the existing `Motor` and `ArmHardware`
interfaces. Use method 1 if the priority is proving the selected motors and
mechanics quickly.

## Evaluation: Technoculture Damiao C++ SDK

The linked Technoculture SDK is not the recommended first integration:

- It opens a serial device rather than Linux SocketCAN.
- It wraps CAN frames in a 30-byte bridge frame beginning with `55 AA`,
  rather than writing `can_frame` objects.
- Its receive path expects a bridge frame beginning with `AA` and ending in
  `55`.
- It requires C++23 and additional `fmt`, `spdlog`, and Catch2 dependencies,
  while Controls currently builds with C++17.
- Several enable/configuration operations contain blocking sleeps and
  retry-by-polling behavior. That needs redesign for the rover control loop.
- Motor feedback is matched through both command and feedback IDs in places;
  the exact adapter framing contract is not documented in the repository
  README.

It can be useful if the hardware team already owns the same Damiao
USB-to-CAN/serial bridge that produced this framing convention. Confirm the
bridge command/receive specification on the actual device before treating
this SDK as reusable. Otherwise, a generic Waveshare/USB serial CAN analyzer,
CANable in serial mode, or SLCAN adapter should not be assumed compatible.

## Decision matrix

| Requirement | OpenArm + SocketCAN | In-repo SocketCAN | Technoculture serial |
| --- | --- | --- | --- |
| Generic USB-CAN hardware | Yes | Yes | No; matching bridge required |
| C++17 compatibility | Yes | Yes | No; upstream uses C++23 |
| Fastest motor bring-up | Best | Medium | Unknown without matching bridge |
| Long-term rover control | Good | Best | Medium after rework |
| Existing diagnostics | `candump` + OpenArm CLI | `candump` + custom status | Adapter/vendor tooling |
| Protocol testability | Good | Best | Some mocks, serial parser tests needed |
| Real-time suitability | Requires loop integration | Designed by the team | Blocking sleeps need rework |
| Vendor lock-in | Low | Lowest | High |

## Recommendation and next actions

1. Procure one **native SocketCAN/`gs_usb`** USB-to-CAN adapter, preferably
   isolated, that is tested at 1 Mb/s and has reliable termination/control
   wiring. Keep a second unit as the bring-up spare or use the second interface
   if the final arm is split across buses.
2. Put the first motor on a bench power supply with a current limit, emergency
   stop, and the bus terminated at both ends.
3. Run `candump can0` and `openarm-can-cli discover`/`monitor` to verify the
   motor's model, command ID, feedback ID, control mode, bitrate, and feedback
   health before integrating with the rover.
4. Confirm the selected Damiao motor model limits from its datasheet and
   compare them with the OpenArm model table. Never drive from guessed torque
   or position ranges.
5. Implement the smallest SocketCAN/OpenArm-backed adapter against the current
   arm motor interface and prove one axis first.
6. Run all four motors on the bench at reduced gains and velocity/torque
   limits. Verify shutdown at the configured feedback watchdog interval,
   plus safe handling of USB unplug, bus-off, and process restart without
   motion.
7. After the adapter works, decide between retaining OpenArm or replacing
   only its internal transport/protocol layer with the in-repository driver.
   Keeping SocketCAN as the boundary makes either choice possible without
   changing hardware.

## References

- [Damiao communication protocol](https://damiao-motor.jia-xie.com/concept/communication-protocol/)
- [Damiao CAN setup and tested USB adapters](https://damiao-motor.jia-xie.com/hardware-setup/can-set-up/)
- [Technoculture Damiao C++ SDK](https://github.com/Technoculture/Damiao-Cpp-SDK)
- [OpenArm CAN library](https://github.com/enactic/openarm_can)
- [OpenArm CAN documentation](https://docs.openarm.dev/software/can/)

