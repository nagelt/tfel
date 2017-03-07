The `@ImposedStress` keyword allows the user to impose the evolution
of a component of the stresses.

This keyword may have one option, which is the way the evolution will
be defined. Two values are accepted: `evolution` and `function`. If no
option is specified, the `evolution` option is chosen.

After the option, the name of the component of the stresses, which
shall be given as string, is expected.

According to the modelling hypothesis selected (see the
`@ModellingHypothesis` keyword), the following components names are
accepted:

- AxisymmetricalGeneralisedPlaneStrain : SRR SZZ STT
- Axisymmetrical                       : SRR SZZ STT SRZ
- PlaneStress                          : SXX SYY     SXY
- PlaneStrain                          : SXX SYY SZZ SXY
- GeneralisedPlaneStrain               : SXX SYY SZZ SXY
- Tridimensional                       : SXX SYY SZZ SXY SXZ SYZ

If the `evolution` option has been selected, the user may specify a
constant evolution by simply giving its value. Otherwise, complex
evolutions can be build using a associative array where the key is the
time and the value the value of the evolution. Between two times, the
values will be interpolated linearly. Prior the first declared time
given, the value corresponding the this first time is used. After the
last time given, the value corresponding this last time is used.

If the `function` option has been selected, a string is expected which
is interpreted as a function of time. The current time is represented
by the variable `t`.

## Example (constant evolution)

~~~~ {.cpp}
@ImposedStress 'SXX' 50.e6;
~~~~~~~~

## Example (linear evolution)

~~~~ {.cpp}
@ImposedStress<evolution> 'SXX' {0.:0.,1:50e6};
~~~~~~~~

## Example (function)

~~~~ {.cpp}
@ImposedStress<function>  'SXX' 's0*sin(t/900.)';
~~~~~~~~

## Note

The off-diagonal terms shall be multiplied by a factor \(\sqrt(2)\).