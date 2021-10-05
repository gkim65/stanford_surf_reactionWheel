# location in julia library packages\POMDPGym\erkmu\src\extra\ReactionWheelPendulum.jl
@with_kw struct ReactionWheelPendulumMDP <: POMDP{Array{Float64}, Float64, Array{Float32}}
    failure_thresh::Union{Nothing, Float64} = deg2rad(90) # if set, defines the operating range fo the pendulum. Episode terminates if abs(θ) is larger than this. 
    θ0 = Distributions.Uniform(deg2rad(-15),deg2rad(15)) # Distribution to sample initial angular position # we can change these to match the initial states we want
    ω0 = Distributions.Uniform(-1, 1) # Distribution to sample initial angular velocity # we might want something really small for this as well
    Rstep = 0 # Reward earned on each step of the simulation
    λcost = 1 # Coefficient to the traditional OpenAIGym Reward
    max_speed::Float64 = 8.

    # max_torque is slightly different for us, since this is how fast we can spin reaction wheels max ESC speed is 180
    max_torque::Float64 = 5.

    dt::Float64 = .05
    g::Float64 = 10.

    m_rw::Float64 = 0.011 # [kg]
    m_arm::Float64 = 0.005 # [kg]
    m_motor::Float64 = 0.02891651 # [kg]

    l_arm_COM::Float64 = 0.1038 # [m]
    l_arm::Float64 = 0.15 # [m]

    # Some equations of motion constants
    I_rw::Float64 = 0.0002475 #2.610735e-5 # [(kg*(m^2))]
    I_arm::Float64 = 7.0569e-7 # [(kg*(m^2))]
    I_motor::Float64 = 0.0006506214749999999 #2.56457e-6 # [(kg*(m^2))]

    
    γ::Float64 = 0.99
    actions::Vector{Float64} = [-1., 1.] # decimal is just to signify floating point
    pixel_observations::Bool = false
    render_fun::Union{Nothing, Function} = nothing
end



angle_normalize(x) = mod((x+π), (2*π)) - π

# change all PendulumMDP into ReactionWheelPendulumMDP
function POMDPs.gen(mdp::ReactionWheelPendulumMDP, s, a, rng::AbstractRNG = Random.GLOBAL_RNG)
    θ, ω = s[1], s[2] # just taking in current state
    dt, g, I_rw, I_arm, I_motor, m_rw, m_arm, m_motor, l_arm_COM, l_arm = mdp.dt, mdp.g, mdp.I_rw, mdp.I_arm, mdp.I_motor, mdp.m_rw, mdp.m_arm, mdp.m_motor, mdp.l_arm_COM, mdp.l_arm


    a = a[1] # get the action - we might need to have it as a vector as a wheel for both magnitude and direction
    @show "blankkkkkkkkkkkkkkkkk"
    @show θ
    a = clamp(a, -mdp.max_torque, mdp.max_torque)
    costs = angle_normalize(θ)^2 + 0.1 * ω^2 + 0.001 * a^2

    # main section calculating angular velocity
    I_tot = (2*(I_rw+I_arm+I_motor))
    k = 2*(m_rw*l_arm + m_motor*l_arm + m_arm*l_arm_COM)*g/I_tot
    h = (a * I_rw)/I_tot
    @show k
    @show h
    ω = ω + (-k * sin(θ) - h) * dt # Possibly revisit the addition of Pi to make sure the 0 is in the right place

    
    θ = θ + ω * dt
    ω = clamp(ω, -mdp.max_speed, mdp.max_speed)
    @show ω
    @show a
    sp = [θ, ω]
    (sp = sp, o=rand(rng, observation(mdp, sp)), r = mdp.Rstep - mdp.λcost*costs) # r is reward
end

function POMDPs.observation(mdp::ReactionWheelPendulumMDP, s)
    o = mdp.pixel_observations ? mdp.render_fun(s) : [angle_normalize(s[1]), s[2]] #[cos(s[1]), sin(s[1]), s[2]]
    Deterministic(Float32.(o))
end

function POMDPs.initialstate(mdp::ReactionWheelPendulumMDP)
    ImplicitDistribution((rng) -> [rand(rng, mdp.θ0), rand(rng, mdp.ω0)]) # we should pick a random angle thats close to balancing >> treat it as if we are holding it up as a human
end

POMDPs.initialobs(mdp::ReactionWheelPendulumMDP, s) = observation(mdp, s)

POMDPs.actions(mdp::ReactionWheelPendulumMDP) = mdp.actions

POMDPs.isterminal(mdp::ReactionWheelPendulumMDP, s) = !isnothing(mdp.failure_thresh) && abs(s[1]) > mdp.failure_thresh
POMDPs.discount(mdp::ReactionWheelPendulumMDP) = mdp.γ

render(mdp::ReactionWheelPendulumMDP, s, a::AbstractArray) = render(mdp, s, a...)
function render(mdp::ReactionWheelPendulumMDP, s, a = 0)
    θ = s[1] + π/2.
    torque = (0.5 + 0.3*cos(θ), 0.5 - 0.3*sin(θ))
    point_array = [(0.5,0.5), torque]
    
    a_rad = abs(a)/10.
    if a < 0
        θstart = -3π/4
        θend = -θstart
        θarr = θend
    else
        θstart = π/4
        θend = -θstart
        θarr = θstart
    end 
    
    
    # Draw the arrow 
    endpt = torque .+ a_rad.*(cos(θarr), sin(θarr))
    uparr = endpt .+ 0.1*a_rad.*(cos(θarr)-sign(a)*sin(θarr), sign(a)*cos(θarr)+sin(θarr))
    dwnarr = endpt .+ 0.1*a_rad.*(-cos(θarr)-sign(a)sin(θarr), sign(a)*cos(θarr)-sin(θarr))
    arrow_array = [[endpt, uparr], [endpt, dwnarr]]
    
    # Just change where the torque arrow is applied >> use both length and angle to correctly place angle on drawing
    # do we code in that we start it from holding it in the middle? or does it start on the table
    img = compose(context(),
        (context(), line(arrow_array), arc(torque..., a_rad, θstart, θend),  linewidth(0.5mm), fillopacity(0.), stroke("red")),
        (context(), circle(torque..., 0.01), fill("blue"), stroke("black")),
        (context(), circle(torque..., 0.1), fill("gray"), fillopacity(0.), stroke("black")),
        (context(), line(point_array), stroke("black"), linewidth(1mm)), # this is the pole
        (context(), rectangle(), fill("white"))
    )
    tmpfilename = tempname()
    img |> PNG(tmpfilename, 10cm, 10cm)
    load(tmpfilename)
end
