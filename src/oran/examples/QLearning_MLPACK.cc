#include <ensmallen.hpp>
#include <mlpack/core.hpp>
#include <mlpack/methods/ann/ffn.hpp>
#include <mlpack/methods/ann/init_rules/gaussian_init.hpp>
#include <mlpack/methods/ann/layer/layer.hpp>
#include <mlpack/methods/ann/loss_functions/mean_squared_error.hpp>
#include <mlpack/methods/reinforcement_learning/environment/acrobot.hpp>
#include <mlpack/methods/reinforcement_learning/environment/cart_pole.hpp>
#include <mlpack/methods/reinforcement_learning/environment/double_pole_cart.hpp>
#include <mlpack/methods/reinforcement_learning/environment/mountain_car.hpp>
#include <mlpack/methods/reinforcement_learning/policy/greedy_policy.hpp>
#include <mlpack/methods/reinforcement_learning/q_learning.hpp>
#include <mlpack/methods/reinforcement_learning/training_config.hpp>

using namespace mlpack;
using namespace mlpack::ann;
using namespace ens;
using namespace mlpack::rl;

int
main()
{
    // Set up the feed forward neural network.
    FFN<MeanSquaredError<>, GaussianInitialization> model(
        MeanSquaredError<>(),
        GaussianInitialization(
            0,
            0.001)); // Gaussian Initialization is how we initialize the weights in the neural
                     // network, with mean 0 and standard deviation 0.001
    model.Add<Linear<>>(4, 128);
    model.Add<ReLULayer<>>();
    model.Add<Linear<>>(128, 128);
    model.Add<ReLULayer<>>();
    model.Add<Linear<>>(128, 2);

    // Set up the policy and replay method.
    GreedyPolicy<CartPole> policy(1.0, 1000, 0.1, 0.99);
    RandomReplay<CartPole> replayMethod(10, 10000);

    TrainingConfig config;
    config.StepSize() = 0.01;
    config.Discount() = 0.9;
    config.TargetNetworkSyncInterval() = 100;
    config.ExplorationSteps() = 100;
    config.DoubleQLearning() = false;
    config.StepLimit() = 200;

    // Set up DQN agent.
    QLearning<CartPole, decltype(model), AdamUpdate, decltype(policy)> agent(
        std::move(config),
        std::move(model),
        std::move(policy),
        std::move(replayMethod));

    while (true)
    {
        double episodeReturn = agent.Episode();
        averageReturn(episodeReturn);
        episodes += 1;
    }

    return 0;
}