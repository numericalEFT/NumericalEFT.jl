@testset "Renormalization" begin
    @testset "CompositeOrder" begin
        # Write your tests here.
        o, c1, c2 = 1, 2, 3
        n = o * 100 + c1 * 10 + c2
        order = CompositeOrder(o, [c1, c2])
        @test order.order == o
        @test order.ct == [c1, c2]
        @test Renorm.short(order) == n
        @test Renorm.short(CompositeOrder(n)) == n
        @test Renorm.short(CompositeOrder("$n")) == n

        @test order == [o, c1, c2]
        @test order == (o, c1, c2)
        @test order != [o, c1, c2, 2]
    end

    @testset "Merge" begin
        o100 = CompositeOrder(1, [0, 0])
        o101 = CompositeOrder(1, [0, 1])
        o011 = CompositeOrder(0, [1, 1])
        o200 = CompositeOrder(2, [0, 0])
        data = Dict(o100 => rand(), o101 => rand(), o011 => rand(), o200 => rand())

        d = Renorm.merge(data, 2)
        @test d[CompositeOrder([1, 0])] ≈ data[o100]
        @test d[CompositeOrder([1, 1])] ≈ data[o011]
        @test d[CompositeOrder([2, 0])] ≈ data[o101] + data[o200]

        dd = Renorm.merge(data, [1, 2])
        @test dd[CompositeOrder([1])] ≈ data[o100]
        @test dd[CompositeOrder([2])] ≈ data[o101] + data[o011] + data[o200]

    end
end