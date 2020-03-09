
#include "physics/simulator.h"

namespace physics
{
    // simple acceleration due to gravity
    float3 acceleration(const state& s, double t)
    {
        return float3(0.0f, -(9.81f), 0.0f);
    }

    derivative evaluate(const state& initial, double t, float dt, const derivative& d)
    {
        state s;
        s.p = initial.p + d.dp * dt;
        s.v = initial.v + d.dv * dt;

        derivative output;
        output.dp = s.v;
        output.dv = acceleration(s, t + dt);
        return output;
    }

    void integrate(state& st, double t, float dt)
    {
        derivative a, b, c, d;
        a = evaluate(st, t, dt, derivative());
        b = evaluate(st, t, dt * 0.5f, a);
        c = evaluate(st, t, dt * 0.5f, b);
        d = evaluate(st, t, dt, c);

        float3 dpdt = (1.0f / 6.0f) * (a.dp + 2.0f * (b.dp + c.dp) + d.dp);
        float3 dvdt = (1.0f / 6.0f) * (a.dv + 2.0f * (b.dv + c.dv) + d.dv);

        st.p = st.p + dpdt * dt;
        st.v = st.v + dvdt * dt;
    }
}
